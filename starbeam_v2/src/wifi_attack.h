#ifndef WIFI_ATTACK_H
#define WIFI_ATTACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Attack types
#define ATTACK_DEAUTH_TARGETED  0
#define ATTACK_DEAUTH_BROADCAST 1
#define ATTACK_BEACON_FLOOD     2
#define ATTACK_PROBE_FLOOD      3
#define ATTACK_PMKID_CAPTURE    4

// Configuration
#define NUM_FRAMES_PER_ATTACK   16
#define MAX_TARGET_NETWORKS     20
#define MAX_FAKE_APS            50
#define PMKID_TIMEOUT           120000  // 2 minutes
#define MAX_PMKID_HISTORY       10      // Store last 10 captures

// Beacon flooding configuration
#define MAX_BEACON_SSIDS        20
#define BEACON_FRAME_SIZE       128
#define BEACON_INTERVAL_TU      100

// Probe flooding configuration
#define PROBE_FRAME_SIZE        64
#define PROBE_BURST_COUNT       10

// IEEE 802.11 frame structures
typedef struct {
  uint8_t frame_control[2];
  uint8_t duration[2];
  uint8_t station[6];
  uint8_t sender[6];
  uint8_t access_point[6];
  uint8_t fragment_sequence[2];
  uint16_t reason;
} __attribute__((packed)) deauth_frame_t;

typedef struct {
  uint8_t frame_control[2];
  uint8_t duration[2];
  uint8_t destination[6];
  uint8_t source[6];
  uint8_t bssid[6];
  uint8_t sequence[2];
} __attribute__((packed)) beacon_header_t;

typedef struct {
  uint8_t frame_control[2];
  uint8_t duration[2];
  uint8_t destination[6];
  uint8_t source[6];
  uint8_t bssid[6];
  uint8_t sequence[2];
} __attribute__((packed)) probe_header_t;

// Captured PMKID data
typedef struct {
  char ssid[33];
  uint8_t ap_mac[6];
  uint8_t station_mac[6];
  uint8_t pmkid[16];
  unsigned long timestamp;
  bool valid;
} pmkid_capture_t;

class WiFiAttack {
public:
    // Lifecycle
    static void init();
    static void deinit();

    // Deauthentication attacks
    static void startDeauthTargeted(int wifi_index, uint16_t reason);
    static void startDeauthBroadcast(uint16_t reason);

    // Beacon flooding
    static void startBeaconFlood(const char** ssid_list, int count);

    // Probe flooding
    static void startProbeFlood(const char* target_ssid);

    // PMKID capture
    static void startPMKIDCapture(int wifi_index);

    // Stop all attacks
    static void stopAttack();

    // Status
    static bool isAttacking() { return attacking; }
    static int getAttackType() { return currentAttackType; }
    static int getFramesSent() { return framesSent; }
    static int getStationsEliminated() { return stationsEliminated; }
    static const pmkid_capture_t* getPMKIDData() { return &pmkidData; }
    static int getCurrentChannel() { return currentChannel; }

    // PMKID history management
    static int getPMKIDHistoryCount() { return pmkidHistoryCount; }
    static const pmkid_capture_t* getPMKIDHistory(int index);
    static void clearPMKIDHistory();

    // Channel control (needed for broadcast deauth mode)
    static void channelHop();

    // Frame builders (public for menu handlers)
    static void buildBeaconFrame(int ap_index);
    static void buildProbeFrame();

private:
    // Packet handlers
    static void IRAM_ATTR deauthSniffer(void* buf, wifi_promiscuous_pkt_type_t type);
    static void IRAM_ATTR pmkidSniffer(void* buf, wifi_promiscuous_pkt_type_t type);

    // Frame builders
    static void buildDeauthFrame(uint8_t* station, uint8_t* ap, uint16_t reason);

    // Helpers
    static void setupWiFiForAttack(uint8_t channel);
    static void enablePromiscuous();
    static void disablePromiscuous();
    static void addPMKIDToHistory(const pmkid_capture_t* capture);

    // State
    static bool attacking;
    static int currentAttackType;
    static int framesSent;
    static int stationsEliminated;
    static int currentChannel;
    static deauth_frame_t deauthFrame;
    static pmkid_capture_t pmkidData;
    static unsigned long attackStartTime;

    // Beacon flooding state
    static char beaconSSIDs[MAX_BEACON_SSIDS][33];
    static uint8_t beaconBSSIDs[MAX_BEACON_SSIDS][6];
    static int beaconSSIDCount;
    static uint8_t beaconChannel;
    static uint8_t beaconFrameBuffer[BEACON_FRAME_SIZE];

    // Probe flooding state
    static char probeTargetSSID[33];
    static uint8_t probeFrameBuffer[PROBE_FRAME_SIZE];

    // PMKID capture state
    static bool pmkidFound;
    static pmkid_capture_t pmkidHistory[MAX_PMKID_HISTORY];
    static int pmkidHistoryCount;
};

// Bypass sanity check for raw frame injection
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3);

#endif
