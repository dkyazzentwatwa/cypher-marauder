// wifi_scanner.h - WiFi Scanner Module for Project Starbeam V2
// Uses ESP32's built-in WiFi for network scanning and channel analysis

#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "display.h"
#include "../config.h"

// ============================================================================
// Configuration Constants
// ============================================================================

#define MAX_NETWORKS 20              // Maximum networks to store
#define WIFI_SCAN_INTERVAL 10000     // WiFi Scanner rescan interval (10 seconds)
#define HEATMAP_SCAN_INTERVAL 2000   // Heatmap rescan interval (2 seconds)
#define DISPLAY_UPDATE_INTERVAL 1000 // Display update throttle (1 second)
#define NUM_WIFI_CHANNELS 14         // WiFi channels 1-14

// ============================================================================
// Data Structures
// ============================================================================

struct WiFiNetworkInfo {
    String ssid;                     // Network name
    String bssid;                    // MAC address string (AA:BB:CC:DD:EE:FF)
    int32_t rssi;                    // Signal strength in dBm
    uint8_t channel;                 // WiFi channel (1-14)
    wifi_auth_mode_t authMode;       // Security type
};

// ============================================================================
// WiFi Scanner Class
// ============================================================================

class WiFiScanner {
public:
    // Lifecycle management
    static void init();
    static void deinit();

    // WiFi Scanner mode (detailed network view)
    static void runScanner();

    // Channel Heatmap mode (spectrum view)
    static void runHeatmap();

    // Terminal list mode (show all networks)
    static void listAll();

    // Web server data access
    static int getNetworkCount() { return networkCount; }
    static const WiFiNetworkInfo* getNetwork(int index);
    static bool isScanning() { return wifiInitialized; }
    static bool isInitialized() { return wifiInitialized; }

    // Background scanning (for WEBSERVER mode)
    static void scanNetworks();

private:
    // Scanning functions
    static void scanForHeatmap();

    // Display functions
    static void displayNetwork(int index);
    static void drawHeatmapSpectrum();

    // Helper functions
    static String generateRssiBar(int32_t rssi);
    static String getSecurityString(wifi_auth_mode_t authMode);

    // State variables
    static WiFiNetworkInfo networks[MAX_NETWORKS];
    static int networkCount;
    static int currentNetworkIndex;
    static unsigned long lastScanTime;
    static unsigned long lastDisplayUpdate;

    // Heatmap data
    static int8_t channelRssi[NUM_WIFI_CHANNELS];
    static uint8_t channelNetworkCount[NUM_WIFI_CHANNELS];
    static unsigned long lastHeatmapScan;

    // WiFi state
    static bool wifiInitialized;
};

#endif // WIFI_SCANNER_H
