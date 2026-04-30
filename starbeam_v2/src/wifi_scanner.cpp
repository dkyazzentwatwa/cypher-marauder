// wifi_scanner.cpp - WiFi Scanner Module Implementation for Project Starbeam V2
// Uses ESP32's built-in WiFi for network scanning and channel analysis

#include "wifi_scanner.h"
#include "input.h"
#include "webserver.h"
#include "terminal.h"

// ============================================================================
// Static Member Initialization
// ============================================================================

WiFiNetworkInfo WiFiScanner::networks[MAX_NETWORKS];
int WiFiScanner::networkCount = 0;
int WiFiScanner::currentNetworkIndex = 0;
unsigned long WiFiScanner::lastScanTime = 0;
unsigned long WiFiScanner::lastDisplayUpdate = 0;

int8_t WiFiScanner::channelRssi[NUM_WIFI_CHANNELS];
uint8_t WiFiScanner::channelNetworkCount[NUM_WIFI_CHANNELS];
unsigned long WiFiScanner::lastHeatmapScan = 0;

bool WiFiScanner::wifiInitialized = false;

// ============================================================================
// Lifecycle Management
// ============================================================================

void WiFiScanner::init() {
    if (wifiInitialized) return;

    Serial.println("Initializing WiFi for scanning...");

    // Initialize WiFi in station mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    wifiInitialized = true;
    Serial.println("WiFi initialized for scanning");
}

void WiFiScanner::deinit() {
    if (!wifiInitialized) return;

    Serial.println("Deinitializing WiFi...");

    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    esp_wifi_stop();
    esp_wifi_deinit();

    wifiInitialized = false;
    Serial.println("WiFi deinitialized");
}

// ============================================================================
// Web Server Data Access
// ============================================================================

const WiFiNetworkInfo* WiFiScanner::getNetwork(int index) {
    if (index >= 0 && index < networkCount) {
        return &networks[index];
    }
    return nullptr;
}

// ============================================================================
// WiFi Scanner Mode (Detailed Network View)
// ============================================================================

void WiFiScanner::runScanner() {
    // Initialize WiFi
    init();

    // Initial scan
    Display::displayInfo("WIFI SCANNER", "Scanning...", "", "");
    scanNetworks();
    currentNetworkIndex = 0;

    Serial.printf("WiFi Scanner: Found %d networks\n", networkCount);

    // Display initial results
    displayNetwork(currentNetworkIndex);

    // Main loop
    while (true) {
        // Periodic rescan every 10 seconds
        if (millis() - lastScanTime > WIFI_SCAN_INTERVAL) {
            int prevIndex = currentNetworkIndex;
            scanNetworks();
            // Keep current index valid after rescan
            if (currentNetworkIndex >= networkCount && networkCount > 0) {
                currentNetworkIndex = networkCount - 1;
            }
            // Display updated results after rescan
            displayNetwork(currentNetworkIndex);
        }

        // Button handling - UP
        if (Input::isButtonPressed(BUTTON_UP)) {
            if (networkCount > 0) {
                currentNetworkIndex = (currentNetworkIndex - 1 + networkCount) % networkCount;
                displayNetwork(currentNetworkIndex);  // Update display on navigation
            }
            delay(150);  // Debounce
        }

        // Button handling - DOWN
        if (Input::isButtonPressed(BUTTON_DOWN)) {
            if (networkCount > 0) {
                currentNetworkIndex = (currentNetworkIndex + 1) % networkCount;
                displayNetwork(currentNetworkIndex);  // Update display on navigation
            }
            delay(150);  // Debounce
        }

        // Keep handleClient() call for compatibility
        if (StarbeamWebServer::isRunning()) {
            StarbeamWebServer::handleClient();
        }

        // Button handling - SELECT to exit
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            break;
        }

        // Terminal stop command
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("WiFi scan stopped via terminal");
            break;
        }

        yield();  // Non-blocking
    }

    // Cleanup
    deinit();
}

void WiFiScanner::scanNetworks() {
    int n = WiFi.scanNetworks();

    if (n == 0) {
        networkCount = 0;
        Serial.println("No networks found");
        return;
    }

    // Limit to MAX_NETWORKS (ESP32 returns sorted by RSSI already)
    networkCount = min(n, MAX_NETWORKS);

    for (int i = 0; i < networkCount; i++) {
        networks[i].ssid = WiFi.SSID(i);
        networks[i].rssi = WiFi.RSSI(i);
        networks[i].channel = WiFi.channel(i);
        networks[i].authMode = WiFi.encryptionType(i);
        networks[i].bssid = WiFi.BSSIDstr(i);
    }

    lastScanTime = millis();
    Serial.printf("Scan complete: %d networks (showing %d)\n", n, networkCount);
}

void WiFiScanner::displayNetwork(int index) {
    if (networkCount == 0) {
        Display::displayInfo("WIFI SCANNER", "No networks", "found", "Scanning...");
        return;
    }

    WiFiNetworkInfo* net = &networks[index];

    // Line 1: SSID with position indicator (truncate if needed)
    String ssidLine = net->ssid;
    if (ssidLine.length() == 0) {
        ssidLine = "[Hidden]";
    } else if (ssidLine.length() > 12) {
        ssidLine = ssidLine.substring(0, 11) + "~";
    }
    ssidLine += " (" + String(index + 1) + "/" + String(networkCount) + ")";

    // Line 2: Channel + RSSI + visual bar
    String rssiBar = generateRssiBar(net->rssi);
    String channelLine = "Ch:" + String(net->channel) + " " +
                         String(net->rssi) + "dBm " + rssiBar;

    // Line 3: Security + partial MAC
    String securityStr = getSecurityString(net->authMode);
    String macShort = net->bssid.substring(0, 8) + "..";
    String secLine = securityStr + " | " + macShort;

    Display::displayInfo("WIFI SCANNER", ssidLine, channelLine, secLine);
}

// ============================================================================
// Channel Heatmap Mode (Spectrum View)
// ============================================================================

void WiFiScanner::runHeatmap() {
    // Initialize WiFi
    init();

    // Initial scan
    Display::displayInfo("WIFI HEATMAP", "Scanning...", "", "");
    scanForHeatmap();
    drawHeatmapSpectrum();

    Serial.println("WiFi Heatmap: Running continuous scan");

    // Main loop - continuous scanning
    while (true) {
        // Continuous scan every 2 seconds
        if (millis() - lastHeatmapScan > HEATMAP_SCAN_INTERVAL) {
            scanForHeatmap();
            drawHeatmapSpectrum();
        }

        // Button handling - SELECT to exit
        if (Input::isButtonPressed(BUTTON_SELECT)) {
            break;
        }

        // Terminal stop command
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("WiFi heatmap stopped via terminal");
            break;
        }

        yield();  // Non-blocking
    }

    // Cleanup
    deinit();
}

void WiFiScanner::scanForHeatmap() {
    // Reset channel data
    for (int i = 0; i < NUM_WIFI_CHANNELS; i++) {
        channelRssi[i] = -100;  // Start with weakest possible
        channelNetworkCount[i] = 0;
    }

    int n = WiFi.scanNetworks();

    for (int i = 0; i < n; i++) {
        uint8_t ch = WiFi.channel(i);
        if (ch >= 1 && ch <= NUM_WIFI_CHANNELS) {
            int idx = ch - 1;
            channelNetworkCount[idx]++;

            // Track peak RSSI (strongest signal on this channel)
            int32_t rssi = WiFi.RSSI(i);
            if (rssi > channelRssi[idx]) {
                channelRssi[idx] = rssi;
            }
        }
    }

    lastHeatmapScan = millis();
    Serial.printf("Heatmap scan complete: %d networks\n", n);
}

void WiFiScanner::drawHeatmapSpectrum() {
    Adafruit_SSD1306& oled = Display::getOled();

    oled.clearDisplay();

    // Title bar
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.print("CHANNEL HEATMAP");
    oled.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

    // Drawing area configuration
    // Y: 12 to 52 (40px height for bars)
    // X: 14 channels across ~120px usable width
    int barWidth = 6;
    int gap = 2;
    int startX = 4;
    int baseY = 52;      // Bottom of graph
    int maxHeight = 36;  // Max bar height in pixels

    // Draw vertical bars for each channel
    for (int ch = 0; ch < NUM_WIFI_CHANNELS; ch++) {
        int x = startX + (ch * (barWidth + gap));

        // Map RSSI (-30 to -100 dBm) to bar height (0 to maxHeight)
        int rssi = channelRssi[ch];
        int height = 0;

        if (rssi > -100) {  // Has signal
            // -30 = full height, -100 = no height
            height = map(rssi, -100, -30, 0, maxHeight);
            height = constrain(height, 2, maxHeight);  // Minimum 2px visible
        }

        // Draw vertical bar
        if (height > 0) {
            oled.fillRect(x, baseY - height, barWidth, height, SSD1306_WHITE);
        }

        // Draw small tick mark at bottom for each channel position
        oled.drawPixel(x + barWidth/2, baseY + 1, SSD1306_WHITE);
    }

    // Channel labels at bottom (1, 6, 11, 14)
    oled.setCursor(startX, 56);
    oled.print("1");

    oled.setCursor(startX + 5 * (barWidth + gap), 56);
    oled.print("6");

    oled.setCursor(startX + 10 * (barWidth + gap), 56);
    oled.print("11");

    // Adjust position for "14" to fit
    oled.setCursor(startX + 13 * (barWidth + gap) - 2, 56);
    oled.print("14");

    oled.display();
}

// ============================================================================
// Terminal List Mode (Show All Networks)
// ============================================================================

void WiFiScanner::listAll() {
    // Initialize WiFi
    init();

    // Scan networks
    Serial.println("Scanning WiFi networks...");
    scanNetworks();

    // Print header
    Serial.println("\n========================================");
    Serial.printf("Found %d WiFi networks:\n", networkCount);
    Serial.println("========================================");

    if (networkCount == 0) {
        Serial.println("No devices found");
    } else {
        // Print each network - compact format (fits in 65 chars)
        for (int i = 0; i < networkCount; i++) {
            WiFiNetworkInfo* net = &networks[i];

            String ssid = net->ssid;
            if (ssid.length() == 0) {
                ssid = "[Hidden]";
            }
            // Truncate long SSIDs
            if (ssid.length() > 14) {
                ssid = ssid.substring(0, 13) + "~";
            }

            // Compact security codes
            String sec;
            switch (net->authMode) {
                case WIFI_AUTH_OPEN: sec = "OP"; break;
                case WIFI_AUTH_WEP: sec = "WE"; break;
                case WIFI_AUTH_WPA_PSK: sec = "W1"; break;
                case WIFI_AUTH_WPA2_PSK:
                case WIFI_AUTH_WPA_WPA2_PSK: sec = "W2"; break;
                case WIFI_AUTH_WPA2_ENTERPRISE: sec = "E2"; break;
                case WIFI_AUTH_WPA3_PSK:
                case WIFI_AUTH_WPA2_WPA3_PSK: sec = "W3"; break;
                default: sec = "??"; break;
            }

            // Format: #. SSID MAC RSSI Ch Sec (no signal bars)
            Serial.printf("%2d. %-14s %s %4ddBm Ch%-2d %s\n",
                i + 1,
                ssid.c_str(),
                net->bssid.c_str(),
                net->rssi,
                net->channel,
                sec.c_str());
        }
    }

    Serial.println("========================================\n");

    // Cleanup
    deinit();
}

// ============================================================================
// Helper Functions
// ============================================================================

String WiFiScanner::generateRssiBar(int32_t rssi) {
    // RSSI scale: -30dBm (excellent) to -90dBm (weak)
    // Map to 5-segment bar
    int bars = 0;

    if (rssi >= -50) bars = 5;       // Excellent
    else if (rssi >= -60) bars = 4;  // Good
    else if (rssi >= -70) bars = 3;  // Fair
    else if (rssi >= -80) bars = 2;  // Weak
    else bars = 1;                    // Very weak

    String bar = "";
    for (int i = 0; i < bars; i++) bar += "#";      // Filled segments
    for (int i = bars; i < 5; i++) bar += ".";      // Empty segments

    return bar;  // Returns "####." or "##..." etc.
}

String WiFiScanner::getSecurityString(wifi_auth_mode_t authMode) {
    switch (authMode) {
        case WIFI_AUTH_OPEN:
            return "OPEN";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA2";
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "WPA2-E";
        case WIFI_AUTH_WPA3_PSK:
            return "WPA3";
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA3";
        default:
            return "???";
    }
}
