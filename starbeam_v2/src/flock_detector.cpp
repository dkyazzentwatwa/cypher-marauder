// flock_detector.cpp - Flock Camera Detector Module for Project Starbeam V2

#include "flock_detector.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include <string.h>

#include "display.h"
#include "input.h"
#include "terminal.h"

namespace {

static const uint8_t kHopChannels[] = {1, 6, 11};
static const size_t kHopChannelCount = sizeof(kHopChannels) / sizeof(kHopChannels[0]);

static const unsigned long kHopIntervalMs = 350;
static const unsigned long kDisplayRefreshMs = 250;
static const uint32_t kStaleMs = 30000;
static const size_t kMaxTrackedDetections = 20;
static const size_t kQueueSize = 32;

static const char* kTargetOuiList[] = {
    "70:c9:4e", "3c:91:80", "d8:f3:bc", "80:30:49", "b8:35:32",
    "14:5a:fc", "74:4c:a1", "08:3a:88", "9c:2f:9d", "c0:35:32",
    "94:08:53", "e4:aa:ea", "f4:6a:dd", "f8:a2:d6", "24:b2:b9",
    "00:f4:8d", "d0:39:57", "e8:d0:fc", "e0:4f:43", "b8:1e:a4",
    "70:08:94", "58:8e:81", "ec:1b:bd", "3c:71:bf", "58:00:e3",
    "90:35:ea", "5c:93:a2", "64:6e:69", "48:27:ea", "a4:cf:12",
    "82:6b:f2"
};

static const size_t kOuiCount = sizeof(kTargetOuiList) / sizeof(kTargetOuiList[0]);
static uint8_t gOuiBytes[kOuiCount][3];
static bool gOuiTableReady = false;

struct DetectionEvent {
    uint8_t mac[6];
    int8_t rssi;
    uint8_t channel;
};

struct TrackedDetection {
    uint8_t mac[6];
    int8_t lastRssi;
    uint8_t lastChannel;
    uint16_t hitCount;
    uint32_t lastSeenMs;
    bool inUse;
};

static volatile DetectionEvent gQueue[kQueueSize];
static volatile size_t gQueueHead = 0;
static volatile size_t gQueueTail = 0;
static volatile uint32_t gDroppedEvents = 0;
static portMUX_TYPE gQueueMux = portMUX_INITIALIZER_UNLOCKED;

static TrackedDetection gDetections[kMaxTrackedDetections];
static uint32_t gTotalHits = 0;
static uint8_t gLastMac[6] = {0};
static int8_t gLastRssi = 0;
static uint8_t gLastChannel = 0;
static uint16_t gLastMacCount = 0;
static bool gHasLastDetection = false;

static bool gInitialized = false;
static uint8_t gChannelMode = 0; // 0=auto hop, 1..3 fixed 1/6/11
static size_t gHopIndex = 0;
static unsigned long gLastHopAt = 0;
static unsigned long gLastDisplayAt = 0;
static uint8_t gActiveChannel = 1;

static bool parseOui(const char* oui, uint8_t out[3]) {
    if (!oui || !out) return false;
    unsigned int b0 = 0, b1 = 0, b2 = 0;
    if (sscanf(oui, "%2x:%2x:%2x", &b0, &b1, &b2) != 3) return false;
    out[0] = static_cast<uint8_t>(b0);
    out[1] = static_cast<uint8_t>(b1);
    out[2] = static_cast<uint8_t>(b2);
    return true;
}

static void initOuiTable() {
    if (gOuiTableReady) return;
    for (size_t i = 0; i < kOuiCount; i++) {
        if (!parseOui(kTargetOuiList[i], gOuiBytes[i])) {
            memset(gOuiBytes[i], 0, sizeof(gOuiBytes[i]));
        }
    }
    gOuiTableReady = true;
}

static bool matchOui(const uint8_t mac[6]) {
    for (size_t i = 0; i < kOuiCount; i++) {
        if (mac[0] == gOuiBytes[i][0] &&
            mac[1] == gOuiBytes[i][1] &&
            mac[2] == gOuiBytes[i][2]) {
            return true;
        }
    }
    return false;
}

static void formatMac(const uint8_t mac[6], char out[18]) {
    snprintf(out, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void setScanChannel(uint8_t channel) {
    if (channel < 1 || channel > 13) return;
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    gActiveChannel = channel;
}

static void IRAM_ATTR enqueueEvent(const uint8_t mac[6], int8_t rssi, uint8_t channel) {
    portENTER_CRITICAL_ISR(&gQueueMux);
    const size_t next = (gQueueHead + 1) % kQueueSize;
    if (next == gQueueTail) {
        gDroppedEvents++;
        portEXIT_CRITICAL_ISR(&gQueueMux);
        return;
    }

    DetectionEvent* evt = (DetectionEvent*)&gQueue[gQueueHead];
    memcpy((void*)evt->mac, mac, 6);
    evt->rssi = rssi;
    evt->channel = channel;
    gQueueHead = next;
    portEXIT_CRITICAL_ISR(&gQueueMux);
}

static void IRAM_ATTR promiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (!buf) return;
    if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;

    const wifi_promiscuous_pkt_t* pkt = reinterpret_cast<const wifi_promiscuous_pkt_t*>(buf);
    const uint8_t* payload = pkt->payload;
    if (!payload) return;

    // 802.11 MAC header: addr2 starts at byte 10.
    const uint8_t* addr2 = payload + 10;
    if (!matchOui(addr2)) return;

    enqueueEvent(addr2, pkt->rx_ctrl.rssi, pkt->rx_ctrl.channel);
}

static int findDetectionSlot(const uint8_t mac[6]) {
    int firstFree = -1;
    uint32_t oldestSeen = UINT32_MAX;
    int oldestIdx = -1;

    for (size_t i = 0; i < kMaxTrackedDetections; i++) {
        if (gDetections[i].inUse) {
            if (memcmp(gDetections[i].mac, mac, 6) == 0) {
                return static_cast<int>(i);
            }
            if (gDetections[i].lastSeenMs < oldestSeen) {
                oldestSeen = gDetections[i].lastSeenMs;
                oldestIdx = static_cast<int>(i);
            }
        } else if (firstFree < 0) {
            firstFree = static_cast<int>(i);
        }
    }

    if (firstFree >= 0) return firstFree;
    return oldestIdx;
}

static void processEvent(const DetectionEvent& evt) {
    const int slot = findDetectionSlot(evt.mac);
    if (slot < 0) return;

    TrackedDetection& det = gDetections[slot];
    if (!det.inUse || memcmp(det.mac, evt.mac, 6) != 0) {
        memset(&det, 0, sizeof(det));
        memcpy(det.mac, evt.mac, 6);
        det.inUse = true;
    }

    det.lastRssi = evt.rssi;
    det.lastChannel = evt.channel;
    det.lastSeenMs = millis();
    det.hitCount = static_cast<uint16_t>(min<uint32_t>(det.hitCount + 1, 65535));

    gTotalHits++;
    memcpy(gLastMac, evt.mac, 6);
    gLastRssi = evt.rssi;
    gLastChannel = evt.channel;
    gLastMacCount = det.hitCount;
    gHasLastDetection = true;
}

static void drainQueue() {
    while (true) {
        DetectionEvent evt;
        bool hasItem = false;

        portENTER_CRITICAL(&gQueueMux);
        if (gQueueTail != gQueueHead) {
            const DetectionEvent* src = (const DetectionEvent*)&gQueue[gQueueTail];
            evt = *src;
            gQueueTail = (gQueueTail + 1) % kQueueSize;
            hasItem = true;
        }
        portEXIT_CRITICAL(&gQueueMux);

        if (!hasItem) break;
        processEvent(evt);
    }
}

static size_t activeDetectionCount() {
    const uint32_t now = millis();
    size_t count = 0;
    for (size_t i = 0; i < kMaxTrackedDetections; i++) {
        if (!gDetections[i].inUse) continue;
        if (now - gDetections[i].lastSeenMs <= kStaleMs) {
            count++;
        }
    }
    return count;
}

static void maybeHopChannel() {
    if (gChannelMode != 0) return;
    if (millis() - gLastHopAt < kHopIntervalMs) return;

    gHopIndex = (gHopIndex + 1) % kHopChannelCount;
    setScanChannel(kHopChannels[gHopIndex]);
    gLastHopAt = millis();
}

static const char* modeLabel() {
    switch (gChannelMode) {
        case 0: return "AUTO";
        case 1: return "CH1";
        case 2: return "CH6";
        case 3: return "CH11";
        default: return "AUTO";
    }
}

static void applyModeChannel() {
    if (gChannelMode == 0) {
        setScanChannel(kHopChannels[gHopIndex]);
        return;
    }
    setScanChannel(kHopChannels[gChannelMode - 1]);
}

static void handleChannelButtons() {
    if (Input::isButtonPressed(BUTTON_UP)) {
        gChannelMode = static_cast<uint8_t>((gChannelMode + 1) % 4);
        applyModeChannel();
    }

    if (Input::isButtonPressed(BUTTON_DOWN)) {
        gChannelMode = static_cast<uint8_t>((gChannelMode == 0) ? 3 : gChannelMode - 1);
        applyModeChannel();
    }
}

static void refreshDisplay() {
    if (millis() - gLastDisplayAt < kDisplayRefreshMs) return;
    gLastDisplayAt = millis();

    char title[28];
    snprintf(title, sizeof(title), "FLOCK %s C%u", modeLabel(), gActiveChannel);

    String line2;
    if (gHasLastDetection) {
        char macText[18];
        formatMac(gLastMac, macText);
        line2 = String(macText).substring(0, 8) + " r" + String(gLastRssi);
    } else {
        line2 = "Waiting for hits...";
    }

    String line3 = "Seen:" + String(activeDetectionCount()) +
                   " Hits:" + String(gTotalHits);
    if (gHasLastDetection) {
        line3 += " #" + String(gLastMacCount);
    }

    String line4 = "[UP/DN]=Ch [SEL]Exit";
    Display::displayInfo(String(title), line2, line3, line4);
}

} // namespace

void FlockDetector::init() {
    if (gInitialized) return;

    initOuiTable();
    memset(gDetections, 0, sizeof(gDetections));
    gQueueHead = 0;
    gQueueTail = 0;
    gDroppedEvents = 0;
    gTotalHits = 0;
    gHasLastDetection = false;
    gChannelMode = 0;
    gHopIndex = 0;
    gLastHopAt = millis();
    gLastDisplayAt = 0;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(&promiscuousCallback);
    setScanChannel(kHopChannels[gHopIndex]);
    esp_wifi_set_promiscuous(true);

    gInitialized = true;
    Serial.println("Flock detector initialized");
}

void FlockDetector::deinit() {
    if (!gInitialized) return;

    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(nullptr);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    esp_wifi_stop();
    esp_wifi_deinit();

    gInitialized = false;
    Serial.println("Flock detector deinitialized");
}

void FlockDetector::runDetector() {
    init();
    Display::displayInfo("FLOCK DETECTOR", "Starting monitor", "No buzzer mode", "");
    delay(500);

    while (true) {
        drainQueue();
        maybeHopChannel();
        handleChannelButtons();
        refreshDisplay();

        if (Input::isButtonPressed(BUTTON_SELECT)) {
            break;
        }

        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("Flock detector stopped via terminal");
            break;
        }

        yield();
    }

    deinit();
}
