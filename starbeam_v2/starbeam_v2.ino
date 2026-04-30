 // Project Starbeam V2 - Main Firmware
// Full implementation with all V1 features
// 100% backward compatible hardware

#include <Wire.h>
#include <SPI.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_bt.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// Project modules
#include "config.h"
#include "types.h"
#include "src/display.h"
#include "src/input.h"

// Radio modules
#include "src/nrf24.h"
#include "src/cc1101.h"
#include "src/recording.h"
#include "src/analyzer.h"
#include "src/wifi_scanner.h"
#include "src/ble_scanner.h"
#include "src/flock_detector.h"
#include "src/webserver.h"
#include "src/wifi_attack.h"
#include "src/terminal.h"

// ============================================================================
// Global State Variables
// ============================================================================

AppState currentState = STATE_MENU;
MenuItem selectedMenuItem = BT_JAM;
int firstVisibleMenuItem = 0;

// CC1101 jamming state
bool jammingMode = false;
byte ccSendBuffer[64] = {0};

// ============================================================================
// Helper Functions
// ============================================================================

void nonBlockingDelay(unsigned long ms) {
    unsigned long start = millis();
    while (millis() - start < ms) {
        yield();  // Allow ESP32 to handle background processes
    }
}

// ============================================================================
// Web Server Mode with Background Scanning Controls
// ============================================================================

void runWebServerMode() {
    // State tracking
    bool wifiScanActive = false;
    bool bleScanActive = false;
    unsigned long lastWiFiScan = 0;
    unsigned long lastBLEScan = 0;

    // Start web server if not running
    if (!StarbeamWebServer::isRunning()) {
        StarbeamWebServer::start();
        delay(2000);  // Show IP screen
    }

    // Main loop
    while (true) {
        // Handle WiFi scanning
        if (wifiScanActive && (millis() - lastWiFiScan >= 10000)) {
            if (WiFiScanner::isInitialized()) {
                WiFiScanner::scanNetworks();
                lastWiFiScan = millis();
            } else {
                WiFiScanner::init();
            }
        }

        // Handle BLE scanning
        if (bleScanActive && (millis() - lastBLEScan >= 10000)) {
            if (!BLEScanner::isInitialized()) {
                BLEScanner::init();
            }
            BLEScanner::performScan();
            lastBLEScan = millis();
        }

        // Display status
        String line1 = "WEB SERVER: ON";
        String line2 = "WiFi: " + String(wifiScanActive ? "ON" : "OFF") +
                       " (" + String(WiFiScanner::getNetworkCount()) + ")";
        String line3 = "BLE: " + String(bleScanActive ? "ON" : "OFF") +
                       " (" + String(BLEScanner::getDeviceCount()) + ")";
        String line4 = "[UP]=WiFi [DN]=BLE";
        Display::displayInfo(line1, line2, line3, line4);

        // Button controls
        if (Input::isButtonPressed(BUTTON_UP)) {
            wifiScanActive = !wifiScanActive;
            if (!wifiScanActive && WiFiScanner::isInitialized()) {
                WiFiScanner::deinit();
            }
            delay(200);
        }

        if (Input::isButtonPressed(BUTTON_DOWN)) {
            bleScanActive = !bleScanActive;
            if (!bleScanActive && BLEScanner::isInitialized()) {
                BLEScanner::deinit();
            }
            delay(200);
        }

        // Long press SELECT to exit
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            unsigned long pressStart = millis();
            while (Input::isButtonPressed(BUTTON_SELECT)) {
                if (millis() - pressStart > 1000) {
                    // Stop web server and cleanup
                    StarbeamWebServer::stop();
                    if (WiFiScanner::isInitialized()) WiFiScanner::deinit();
                    if (BLEScanner::isInitialized()) BLEScanner::deinit();
                    return;
                }
                yield();
            }
        }

        // Process web requests
        StarbeamWebServer::handleClient();
        yield();
    }
}

// ============================================================================
// Security Testing Mode - Targeted Deauth Attack
// ============================================================================

void runDeauthTargetMode() {
    // Show legal warning
    Display::displayLegalWarning();
    while (!Input::isButtonPressed(BUTTON_SELECT)) {
        yield();
    }
    delay(500);

    // Scan for networks
    Display::displayInfo("DEAUTH TARGET", "Scanning...", "", "");
    int networkCount = WiFi.scanNetworks();

    if (networkCount == 0) {
        Display::displayInfo("DEAUTH TARGET", "No networks found", "", "[SELECT] Exit");
        while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
        return;
    }

    // Network selection
    int selectedNetwork = 0;
    while (true) {
        String line1 = "Select Target:";
        String line2 = WiFi.SSID(selectedNetwork);
        String line3 = "Ch:" + String(WiFi.channel(selectedNetwork)) +
                       " RSSI:" + String(WiFi.RSSI(selectedNetwork));
        String line4 = "[UP/DN][SEL]Start";
        Display::displayInfo(line1, line2, line3, line4);

        if (Input::isButtonPressed(BUTTON_UP)) {
            selectedNetwork = (selectedNetwork + 1) % networkCount;
            delay(200);
        }
        if (Input::isButtonPressed(BUTTON_DOWN)) {
            selectedNetwork = (selectedNetwork - 1 + networkCount) % networkCount;
            delay(200);
        }
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            break;
        }
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("Deauth target selection cancelled via terminal");
            return;
        }
        yield();
    }

    // Start attack with reason code 7 (Class 3 frame from nonassociated STA)
    WiFiAttack::init();
    WiFiAttack::startDeauthTargeted(selectedNetwork, 7);

    // Attack loop
    while (WiFiAttack::isAttacking()) {
        String line1 = "DEAUTH ACTIVE";
        String line2 = WiFi.SSID(selectedNetwork);
        String line3 = "Frames: " + String(WiFiAttack::getFramesSent());
        String line4 = "Clients: " + String(WiFiAttack::getStationsEliminated());
        Display::displayInfo(line1, line2, line3, line4);

        // Long press to stop
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            unsigned long pressStart = millis();
            while (Input::isButtonPressed(BUTTON_SELECT)) {
                if (millis() - pressStart > 1000) {
                    WiFiAttack::stopAttack();
                    break;
                }
                yield();
            }
        }

        // Terminal stop command
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("Deauth attack stopped via terminal");
            WiFiAttack::stopAttack();
            break;
        }

        yield();
    }

    WiFiAttack::deinit();
}

// ============================================================================
// Security Testing Mode - Broadcast Deauth Attack
// ============================================================================

void runDeauthBroadcastMode() {
    // Show legal warning with extra emphasis
    Display::displayLegalWarning();
    while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
    delay(500);

    // Extra warning for broadcast mode
    Display::displayInfo("WARNING!", "This will disrupt", "ALL nearby WiFi", "[SEL] Confirm");
    while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
    delay(500);

    WiFiAttack::init();
    WiFiAttack::startDeauthBroadcast(7);

    // Attack loop with channel hopping
    while (WiFiAttack::isAttacking()) {
        String line1 = "DEAUTH ALL";
        String line2 = "Channel: " + String(WiFiAttack::getCurrentChannel());
        String line3 = "Frames: " + String(WiFiAttack::getFramesSent());
        String line4 = "[HOLD SEL] Stop";
        Display::displayInfo(line1, line2, line3, line4);

        // Channel hopping for broadcast mode
        static unsigned long lastHop = 0;
        if (millis() - lastHop > 10) {
            WiFiAttack::channelHop();
            lastHop = millis();
        }

        // Long press to stop
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            unsigned long pressStart = millis();
            while (Input::isButtonPressed(BUTTON_SELECT)) {
                if (millis() - pressStart > 1000) {
                    WiFiAttack::stopAttack();
                    break;
                }
                yield();
            }
        }

        // Terminal stop command
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("Broadcast deauth stopped via terminal");
            WiFiAttack::stopAttack();
            break;
        }

        yield();
    }

    WiFiAttack::deinit();
}

// Beacon flooding mode
void runBeaconFloodMode() {
    // Show legal warning
    Display::displayLegalWarning();
    while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
    delay(500);

    // List of fake SSIDs for beacon flooding
    const char* fake_ssids[20] = {
        "Free WiFi", "Airport WiFi", "Guest Network", "Starbucks WiFi",
        "xfinitywifi", "ATT WiFi", "Verizon WiFi", "Public WiFi",
        "Hotel Guest", "Conference WiFi", "Open Network", "Guest",
        "Visitor WiFi", "Free Internet", "WiFi Access", "Network",
        "Internet", "WiFi", "Wireless", "Public"
    };

    WiFiAttack::init();
    WiFiAttack::startBeaconFlood(fake_ssids, 20);

    // Attack loop
    while (WiFiAttack::isAttacking()) {
        String line1 = "BEACON FLOOD";
        String line2 = "APs: 20 | Ch: 6";
        String line3 = "Sent: " + String(WiFiAttack::getFramesSent());
        String line4 = "[HOLD SEL] Stop";
        Display::displayInfo(line1, line2, line3, line4);

        // Transmit beacons in round-robin
        for (int i = 0; i < 20 && WiFiAttack::isAttacking(); i++) {
            WiFiAttack::buildBeaconFrame(i);
            delay(10);  // 10ms between frames = ~100 beacons/sec
            if (i % 10 == 0) yield();  // Prevent watchdog timeout
        }

        // Long press to stop
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            unsigned long pressStart = millis();
            while (Input::isButtonPressed(BUTTON_SELECT)) {
                if (millis() - pressStart > 1000) {
                    WiFiAttack::stopAttack();
                    break;
                }
                yield();
            }
        }

        // Terminal stop command
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("Beacon flood stopped via terminal");
            WiFiAttack::stopAttack();
            break;
        }
    }

    WiFiAttack::deinit();
}

// Probe request flooding mode
void runProbeFloodMode() {
    // Show legal warning
    Display::displayLegalWarning();
    while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
    delay(500);

    WiFiAttack::init();
    WiFiAttack::startProbeFlood("");  // Wildcard mode (empty SSID)

    // Attack loop
    while (WiFiAttack::isAttacking()) {
        String line1 = "PROBE FLOOD";
        String line2 = "Mode: Wildcard";
        String line3 = "Sent: " + String(WiFiAttack::getFramesSent());
        String line4 = "[HOLD SEL] Stop";
        Display::displayInfo(line1, line2, line3, line4);

        // Burst transmission
        for (int i = 0; i < PROBE_BURST_COUNT && WiFiAttack::isAttacking(); i++) {
            WiFiAttack::buildProbeFrame();
        }
        delay(2);  // 2ms between bursts = ~500 probes/sec

        // Long press to stop
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            unsigned long pressStart = millis();
            while (Input::isButtonPressed(BUTTON_SELECT)) {
                if (millis() - pressStart > 1000) {
                    WiFiAttack::stopAttack();
                    break;
                }
                yield();
            }
        }

        // Terminal stop command
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("Probe flood stopped via terminal");
            WiFiAttack::stopAttack();
            break;
        }

        yield();
    }

    WiFiAttack::deinit();
}

// PMKID capture mode
void runPMKIDCaptureMode() {
    // Show legal warning
    Display::displayLegalWarning();
    while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
    delay(500);

    // Scan for WPA2/WPA3 networks only
    Display::displayInfo("PMKID CAPTURE", "Scanning...", "", "");
    int networkCount = WiFi.scanNetworks();

    // Filter for WPA2/WPA3 networks
    int wpa2Indices[20];
    int wpa2Count = 0;
    for (int i = 0; i < networkCount && wpa2Count < 20; i++) {
        wifi_auth_mode_t auth = WiFi.encryptionType(i);
        if (auth >= WIFI_AUTH_WPA2_PSK) {
            wpa2Indices[wpa2Count++] = i;
        }
    }

    if (wpa2Count == 0) {
        Display::displayInfo("PMKID CAPTURE", "No WPA2/3 APs", "", "[SEL] Exit");
        while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
        return;
    }

    // Network selection
    int selectedIdx = 0;
    while (true) {
        int netIdx = wpa2Indices[selectedIdx];
        String line1 = "Select Target:";
        String line2 = WiFi.SSID(netIdx);
        String line3 = "Ch:" + String(WiFi.channel(netIdx));
        String line4 = "[UP/DN][SEL]";
        Display::displayInfo(line1, line2, line3, line4);

        if (Input::isButtonPressed(BUTTON_UP)) {
            selectedIdx = (selectedIdx + 1) % wpa2Count;
            delay(200);
        }
        if (Input::isButtonPressed(BUTTON_DOWN)) {
            selectedIdx = (selectedIdx - 1 + wpa2Count) % wpa2Count;
            delay(200);
        }
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            break;
        }
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("PMKID target selection cancelled via terminal");
            return;
        }
        yield();
    }

    WiFiAttack::init();
    WiFiAttack::startPMKIDCapture(wpa2Indices[selectedIdx]);

    // Capture loop
    unsigned long startTime = millis();
    while (WiFiAttack::isAttacking()) {
        const pmkid_capture_t* data = WiFiAttack::getPMKIDData();

        if (data->valid) {
            Display::displayInfo("PMKID CAPTURED!", data->ssid, "Check Serial", "[SEL] Exit");
            while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
            break;
        }

        unsigned long elapsed = (millis() - startTime) / 1000;
        String line1 = "PMKID CAPTURE";
        String line2 = "Listening...";
        String line3 = "Time: " + String(elapsed) + "s";
        String line4 = "[SEL] Cancel";
        Display::displayInfo(line1, line2, line3, line4);

        // 2 minute timeout
        if (elapsed > 120) {
            Display::displayInfo("TIMEOUT", "No handshake", "", "[SEL] Exit");
            while (!Input::isButtonPressed(BUTTON_SELECT)) yield();
            break;
        }

        // Button press to cancel
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            WiFiAttack::stopAttack();
            break;
        }

        // Terminal stop command
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("PMKID capture stopped via terminal");
            WiFiAttack::stopAttack();
            break;
        }

        yield();
    }

    WiFiAttack::deinit();
}

// ============================================================================
// Execute Selected Menu Item - Main Feature Handler
// ============================================================================

void executeSelectedMenuItem() {
    switch (selectedMenuItem) {
        case BT_JAM:
            currentState = STATE_BT_JAM;
            Serial.println("BT JAM selected");
            Display::displayInfo("BT JAMMER", "TURN ON LEFT SWITCH", "Starting....", "");
            NRF24Radio::initVSPI();
            NRF24Radio::initHSPI();
            nonBlockingDelay(2000);
            Display::displayInfo("BT JAMMER", "RADIOS ACTIVE", "Running....", "");
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                NRF24Radio::btJam();
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
                Serial.println("BT JAM stopped via terminal");
            }
            break;

        case WIFI_JAM:
            currentState = STATE_WIFI_JAM;
            Serial.println("WIFI JAM selected");
            Display::displayInfo("WIFI JAMMER", "TURN ON LEFT SWITCH", "Starting....", "");
            NRF24Radio::initVSPI();
            NRF24Radio::initHSPI();
            nonBlockingDelay(2000);
            Display::displayInfo("WIFI JAMMER", "RADIOS ACTIVE", "Running....", "");
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                NRF24Radio::wifiJam();
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
                Serial.println("WIFI JAM stopped via terminal");
            }
            break;

        case DRONE_JAM:
            currentState = STATE_DRONE_JAM;
            Serial.println("DRONE JAM selected");
            Display::displayInfo("DRONE JAMMER", "TURN ON LEFT SWITCH", "Starting....", "");
            NRF24Radio::initVSPI();
            NRF24Radio::initHSPI();
            nonBlockingDelay(2000);
            Display::displayInfo("DRONE JAMMER", "RADIOS ACTIVE", "Running....", "");
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                NRF24Radio::droneJam();
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
                Serial.println("DRONE JAM stopped via terminal");
            }
            break;

        case NRF_SCAN:
            currentState = STATE_NRF_SCAN;
            Serial.println("NRF_SCAN selected");
            Display::displayInfo("NRF24 SCAN", "TURN ON LEFT SWITCH", "Scanning....", "");
            analyzerSetup();
            nonBlockingDelay(4000);
            break;

        case WIFI_SCAN:
            currentState = STATE_WIFI_SCAN;
            Serial.println("WIFI_SCAN selected");
            WiFiScanner::runScanner();
            break;

        case WIFI_HEATMAP:
            currentState = STATE_WIFI_HEATMAP;
            Serial.println("WIFI_HEATMAP selected");
            WiFiScanner::runHeatmap();
            break;

        case BLE_SCAN:
            currentState = STATE_BLE_SCAN;
            Serial.println("BLE_SCAN selected");
            BLEScanner::init();
            BLEScanner::runScanner();
            BLEScanner::deinit();
            break;

        case FLOCK_DETECTOR:
            currentState = STATE_FLOCK_DETECTOR;
            Serial.println("FLOCK_DETECTOR selected");
            FlockDetector::runDetector();
            break;

        case WEBSERVER_ON:
            currentState = STATE_WEBSERVER;
            Serial.println("WEBSERVER_ON selected");
            if (!StarbeamWebServer::isRunning()) {
                StarbeamWebServer::start();
                Display::displayInfo("Web Server", "Starting...", "", "");
                delay(1000);
                Display::displayInfo("Web Server", "STARTED", "IP: " + StarbeamWebServer::getIP(), "[SELECT] Back");
                while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                    yield();
                }
            } else {
                Display::displayInfo("Web Server", "Already running", "IP: " + StarbeamWebServer::getIP(), "[SELECT] Back");
                while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                    yield();
                }
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case WEBSERVER_OFF:
            currentState = STATE_WEBSERVER;
            Serial.println("WEBSERVER_OFF selected");
            if (StarbeamWebServer::isRunning()) {
                StarbeamWebServer::stop();
                Display::displayInfo("Web Server", "Stopping...", "", "");
                delay(500);
                Display::displayInfo("Web Server", "STOPPED", "", "[SELECT] Back");
                while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                    yield();
                }
            } else {
                Display::displayInfo("Web Server", "Not running", "", "[SELECT] Back");
                while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                    yield();
                }
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case WEBSERVER_STATUS:
            currentState = STATE_WEBSERVER;
            Serial.println("WEBSERVER_STATUS selected");
            if (StarbeamWebServer::isRunning()) {
                Display::displayInfo("Web Server", "Status: RUNNING", "IP: " + StarbeamWebServer::getIP(), "[SELECT] Back");
            } else {
                Display::displayInfo("Web Server", "Status: STOPPED", "", "[SELECT] Back");
            }
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                yield();
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case SEC_DEAUTH_TARGET:
            currentState = STATE_SEC_DEAUTH_TARGET;
            Serial.println("SEC_DEAUTH_TARGET selected");
            runDeauthTargetMode();
            break;

        case SEC_DEAUTH_ALL:
            currentState = STATE_SEC_DEAUTH_ALL;
            Serial.println("SEC_DEAUTH_ALL selected");
            runDeauthBroadcastMode();
            break;

        case SEC_BEACON_FLOOD:
            currentState = STATE_SEC_BEACON_FLOOD;
            Serial.println("SEC_BEACON_FLOOD selected");
            runBeaconFloodMode();
            break;

        case SEC_PROBE_FLOOD:
            currentState = STATE_SEC_PROBE_FLOOD;
            Serial.println("SEC_PROBE_FLOOD selected");
            runProbeFloodMode();
            break;

        case SEC_PMKID_CAPTURE:
            currentState = STATE_SEC_PMKID_CAPTURE;
            Serial.println("SEC_PMKID_CAPTURE selected");
            runPMKIDCaptureMode();
            break;

        case TEST_NRF:
            currentState = STATE_TEST_NRF;
            Serial.println("TEST_NRF selected");
            Display::displayInfo("NRF24 TEST", "TURN ON LEFT SWITCH", "Starting....", "");
            NRF24Radio::initVSPI();
            nonBlockingDelay(2000);
            break;

        case TEST_CC1101:
            currentState = STATE_TEST_CC1101;
            Serial.println("TEST_CC1101 selected");
            Display::displayInfo("CC1101 TEST", "TURN ON RIGHT SWITCH", "Starting....", "");
            CC1101Radio::init();
            CC1101Radio::init2();
            if (CC1101Radio::checkConnection()) {
                Display::displayInfo("CC1101 TEST", "CC1 OK!", "", "");
            } else {
                Display::displayInfo("CC1101 TEST", "CC1 FAILED!", "CHECK WIRING", "");
            }
            nonBlockingDelay(2000);
            if (CC1101Radio::checkConnection2()) {
                Display::displayInfo("CC1101 TEST", "CC2 OK!", "", "");
            } else {
                Display::displayInfo("CC1101 TEST", "CC2 FAILED!", "CHECK WIRING", "");
            }
            nonBlockingDelay(2000);
            break;

        case TEST_HSPI:
            currentState = STATE_TEST_NRF_5;
            Serial.println("TEST_HSPI selected");
            Display::displayInfo("HSPI TEST", "TURN ON LEFT SWITCH", "Starting....", "");
            NRF24Radio::initHSPI();
            nonBlockingDelay(2000);
            break;

        case CC1_JAM:
            currentState = STATE_CC1_JAM;
            Serial.println("CC1 JAM selected");
            Display::displayInfo("433MHz JAMMER", "Activating Radio", "Starting....", "");
            jammingMode = true;
            while (jammingMode) {
                if (Input::isButtonPressed(BUTTON_SELECT) || Terminal::stopRequested()) {
                    Serial.println("Exiting Jamming Mode");
                    jammingMode = false;
                    if (Terminal::stopRequested()) {
                        Terminal::clearStopFlag();
                        Serial.println("CC1 JAM stopped via terminal");
                    }
                    break;
                }
                Display::displayInfo("CC1101 JAMMER", "RADIOS ACTIVE", "Running....", "");
                randomSeed(analogRead(0));
                for (int i = 0; i < 60; i++) {
                    ccSendBuffer[i] = (byte)random(255);
                }
                CC1101Radio::getCC1().SendData(ccSendBuffer, 60);
                CC1101Radio::getCC2().SendData(ccSendBuffer, 60);
                nonBlockingDelay(10);
            }
            break;

        case CC1_SINGLE:
            currentState = STATE_CC1_SINGLE;
            Serial.println("CC1 SINGLE selected");
            Display::displayInfo("CC#1 JAMMER", "Activating Radio", "Starting....", "");
            jammingMode = true;
            while (jammingMode) {
                if (Input::isButtonPressed(BUTTON_SELECT) || Terminal::stopRequested()) {
                    jammingMode = false;
                    if (Terminal::stopRequested()) {
                        Terminal::clearStopFlag();
                        Serial.println("CC1 SINGLE stopped via terminal");
                    }
                    break;
                }
                Display::displayInfo("CC1101 JAMMER", "CC#1 ACTIVE", "Running....", "");
                randomSeed(analogRead(0));
                for (int i = 0; i < 60; i++) {
                    ccSendBuffer[i] = (byte)random(255);
                }
                CC1101Radio::getCC1().SendData(ccSendBuffer, 60);
                nonBlockingDelay(10);
            }
            break;

        case CC2_SINGLE:
            currentState = STATE_CC2_SINGLE;
            Serial.println("CC2 SINGLE selected");
            Display::displayInfo("CC#2 JAMMER", "Activating Radio", "Starting....", "");
            jammingMode = true;
            while (jammingMode) {
                if (Input::isButtonPressed(BUTTON_SELECT) || Terminal::stopRequested()) {
                    jammingMode = false;
                    if (Terminal::stopRequested()) {
                        Terminal::clearStopFlag();
                        Serial.println("CC2 SINGLE stopped via terminal");
                    }
                    break;
                }
                Display::displayInfo("CC1101 JAMMER", "CC#2 ACTIVE", "Running....", "");
                randomSeed(analogRead(0));
                for (int i = 0; i < 60; i++) {
                    ccSendBuffer[i] = (byte)random(255);
                }
                CC1101Radio::getCC2().SendData(ccSendBuffer, 60);
                nonBlockingDelay(10);
            }
            break;

        case CC_SCAN:
            currentState = STATE_CC_SCAN;
            Serial.println("CC_SCAN selected");
            Display::displayInfo("CC_SCAN", "Scanning frequencies", "Scanning....", "");
            CC1101Radio::scan(433.60, 434.20);
            break;

        case REC_RAW:
            currentState = STATE_REC_RAW;
            Serial.println("REC_RAW selected");
            Display::displayInfo("REC_RAW", "Recording raw data", "Recording....", "");
            Recording::recordRawData(100);
            break;

        case PLAY_RAW:
            currentState = STATE_PLAY_RAW;
            Serial.println("PLAY_RAW selected");
            Display::displayInfo("PLAY_RAW", "Playing raw data", "Playing....", "");
            Recording::playRawData(100);
            break;

        case SHOW_RAW:
            currentState = STATE_SHOW_RAW;
            Serial.println("SHOW_RAW selected");
            Display::displayInfo("SHOW_RAW", "Showing raw data", "Raw data....", "");
            Recording::showRawData();
            nonBlockingDelay(3000);
            break;

        case SHOW_BUFF:
            currentState = STATE_SHOW_BUFF;
            Serial.println("SHOW_BUFF selected");
            Display::displayInfo("SHOW_BUFF", "Showing buffer", "Bit data....", "");
            Recording::showBitData();
            nonBlockingDelay(3000);
            break;

        case FLUSH_BUFF:
            currentState = STATE_FLUSH_BUFF;
            Serial.println("FLUSH_BUFF selected");
            Display::displayInfo("FLUSH_BUFF", "Clearing buffer", "Clearing....", "");
            Recording::flushBuffer();
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                Display::displayInfo("FLUSH_BUFF", "Buffer cleared!", "Press SELECT", "to return");
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case GET_RSSI:
            currentState = STATE_GET_RSSI;
            Serial.println("GET_RSSI selected");
            {
                float rssi = CC1101Radio::getRssi();
                int lqi = CC1101Radio::getLqi();
                char rssiStr[16];
                char lqiStr[16];
                snprintf(rssiStr, 16, "RSSI: %.1f dBm", rssi);
                snprintf(lqiStr, 16, "LQI: %d", lqi);
                Display::displayInfo("GET_RSSI", rssiStr, lqiStr, "");
                Serial.printf("RSSI: %.1f dBm, LQI: %d\n", rssi, lqi);
            }
            nonBlockingDelay(3000);
            break;

        case STOP_ALL:
            currentState = STATE_STOP_ALL;
            Serial.println("STOP_ALL selected");
            Display::displayInfo("STOP_ALL", "Stopping all", "modes....", "");
            jammingMode = false;
            CC1101Radio::stopJamming();
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                Display::displayInfo("STOP_ALL", "All stopped!", "Press SELECT", "to return");
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case RESET_CC:
            currentState = STATE_RESET_CC;
            Serial.println("RESET_CC selected");
            Display::displayInfo("RESET_CC", "Resetting CC1101", "Initializing....", "");
            CC1101Radio::init();
            CC1101Radio::init2();
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                Display::displayInfo("RESET_CC", "CC1101 Reset!", "Press SELECT", "to return");
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case SET_43440:
            currentState = STATE_SET_43440;
            Serial.println("SET_43440 selected");
            Display::displayInfo("SET_43440", "FREQ SET", "434.40MHz", "");
            CC1101Radio::setMhz(434.40);
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                Display::displayInfo("SET_43440", "Freq: 434.40MHz", "Press SELECT", "to return");
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case SET_43430:
            currentState = STATE_SET_43430;
            Serial.println("SET_43430 selected");
            Display::displayInfo("SET_43430", "FREQ SET", "434.30MHz", "");
            CC1101Radio::setMhz(434.30);
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                Display::displayInfo("SET_43430", "Freq: 434.30MHz", "Press SELECT", "to return");
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case SET_43400:
            currentState = STATE_SET_43400;
            Serial.println("SET_43400 selected");
            Display::displayInfo("SET_43400", "FREQ SET", "434.00MHz", "");
            CC1101Radio::setMhz(434.00);
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                Display::displayInfo("SET_43400", "Freq: 434.00MHz", "Press SELECT", "to return");
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case SET_43390:
            currentState = STATE_SET_43390;
            Serial.println("SET_43390 selected");
            Display::displayInfo("SET_43390", "FREQ SET", "433.90MHz", "");
            CC1101Radio::setMhz(433.90);
            while (!Input::isButtonPressed(BUTTON_SELECT) && !Terminal::stopRequested()) {
                Display::displayInfo("SET_43390", "Freq: 433.90MHz", "Press SELECT", "to return");
            }
            if (Terminal::stopRequested()) {
                Terminal::clearStopFlag();
            }
            break;

        case SETTINGS:
            Display::displayInfo("Settings", "Not yet", "implemented", "");
            nonBlockingDelay(2000);
            break;

        case HELP:
            Display::displayInfo("Help", "Project Starbeam", "V2 Firmware", "by Cypher");
            nonBlockingDelay(3000);
            break;

        default:
            Display::displayInfo("Unknown", "Menu item", "not found", "");
            nonBlockingDelay(2000);
            break;
    }
}

// ============================================================================
// Setup Function
// ============================================================================

void setup() {
    Serial.begin(115200);

    // Initialize terminal interface
    Terminal::init();

    Serial.println("\n\nProject Starbeam V2");
    Serial.println("Full implementation - 100% V1 hardware compatible");

    delay(1000);

    // Initialize EEPROM
    EEPROM.begin(EEPROM_SIZE);

    // Initialize display
    Display::init();

    // Disable unnecessary wireless interfaces (like V1)
    esp_bt_controller_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();

    // Initialize input
    Input::init();

    // Initialize web server (doesn't start AP yet)
    StarbeamWebServer::init();

    // Display title screen
    Display::displayTitleScreen();
    delay(2000);

    // Initialize CC1101 radios at startup (like V1)
    CC1101Radio::init();
    CC1101Radio::init2();

    // Display CC1101 connection status
    if (CC1101Radio::checkConnection()) {
        Serial.println("CC1101 #1: Connection OK");
        Display::displayInfo("CC1101 #1", "Connection OK", "", "");
    } else {
        Serial.println("CC1101 #1: Connection ERROR");
        Display::displayInfo("CC1101 #1", "ERROR!", "Check wiring", "");
    }
    delay(1500);

    if (CC1101Radio::checkConnection2()) {
        Serial.println("CC1101 #2: Connection OK");
        Display::displayInfo("CC1101 #2", "Connection OK", "", "");
    } else {
        Serial.println("CC1101 #2: Connection ERROR");
        Display::displayInfo("CC1101 #2", "ERROR!", "Check wiring", "");
    }
    delay(1500);

    // Initialize recording module
    Recording::init();

    // Draw initial menu
    Display::drawMenu(selectedMenuItem, firstVisibleMenuItem);

    Serial.println("Setup complete - ready!");
    Serial.printf("Free heap: %lu bytes\n", ESP.getFreeHeap());
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    // ========================================================================
    // STEP 1: Process serial input (non-blocking)
    // ========================================================================
    Terminal::processInput();

    // ========================================================================
    // STEP 2: Check for serial command and execute it
    // ========================================================================
    if (Terminal::hasCommand()) {
        MenuItem cmd = Terminal::getCommand();
        Terminal::clearCommand();

        // Exit current operation if running
        if (currentState != STATE_MENU) {
            Serial.println("Serial command - exiting current operation");
            currentState = STATE_MENU;
            delay(100);
        }

        // Execute command
        selectedMenuItem = cmd;
        executeSelectedMenuItem();
        currentState = STATE_MENU;
        Display::drawMenu(selectedMenuItem, firstVisibleMenuItem);
        Input::setButtonPressedState(false);
    }

    // ========================================================================
    // STEP 3: Check stop signal
    // ========================================================================
    if (Terminal::stopRequested() && currentState != STATE_MENU) {
        Serial.println("Stop command received");
        Terminal::clearStopFlag();
        jammingMode = false;
        currentState = STATE_MENU;
        Display::drawMenu(selectedMenuItem, firstVisibleMenuItem);
        return;
    }

    // ========================================================================
    // STEP 4: Existing state machine
    // ========================================================================
    switch (currentState) {
        case STATE_MENU:
            // Handle menu navigation
            Input::handleMenuSelection(selectedMenuItem, firstVisibleMenuItem);

            // Check if SELECT was pressed (to enter a mode)
            if (Input::getButtonPressedState() && Input::isButtonPressed(BUTTON_SELECT)) {
                Serial.printf("Selected menu item: %d\n", selectedMenuItem);
                executeSelectedMenuItem();

                // Return to menu after feature execution
                currentState = STATE_MENU;
                Display::drawMenu(selectedMenuItem, firstVisibleMenuItem);
                Input::setButtonPressedState(false);
            }
            break;

        case STATE_NRF_SCAN:
            // Run analyzer loop
            analyzerLoop();
            if (Input::isButtonPressed(BUTTON_SELECT)) {
                currentState = STATE_MENU;
                Display::drawMenu(selectedMenuItem, firstVisibleMenuItem);
                nonBlockingDelay(500);
            }
            break;

        // All other states return to menu on SELECT
        case STATE_BT_JAM:
        case STATE_DRONE_JAM:
        case STATE_WIFI_JAM:
        case STATE_CC1_JAM:
        case STATE_CC_SCAN:
        case STATE_WIFI_SCAN:
        case STATE_WIFI_HEATMAP:
        case STATE_FLOCK_DETECTOR:
        case STATE_CC1_SINGLE:
        case STATE_CC2_SINGLE:
        case STATE_REC_RAW:
        case STATE_PLAY_RAW:
        case STATE_SHOW_RAW:
        case STATE_SHOW_BUFF:
        case STATE_FLUSH_BUFF:
        case STATE_GET_RSSI:
        case STATE_STOP_ALL:
        case STATE_RESET_CC:
        case STATE_SET_43392:
        case STATE_SET_43400:
        case STATE_SET_43390:
        case STATE_SET_43387:
        case STATE_SET_38800:
        case STATE_SET_39000:
        case STATE_SET_40000:
        case STATE_SET_434500:
        case STATE_SET_43440:
        case STATE_SET_43430:
        case STATE_TEST_NRF:
        case STATE_TEST_NRF_5:
        case STATE_TEST_CC1101:
            if (Input::isButtonPressed(BUTTON_SELECT)) {
                currentState = STATE_MENU;
                Display::drawMenu(selectedMenuItem, firstVisibleMenuItem);
                nonBlockingDelay(500);
            }
            break;
    }

    yield();  // Allow other tasks
}
