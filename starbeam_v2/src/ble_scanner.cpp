#include "ble_scanner.h"
#include "display.h"
#include "input.h"
#include "config.h"
#include "webserver.h"
#include "terminal.h"

// Initialize static member variables
BLEDeviceInfo BLEScanner::devices[MAX_BLE_DEVICES];
int BLEScanner::deviceCount = 0;
int BLEScanner::currentDeviceIndex = 0;
bool BLEScanner::detailMode = false;
int BLEScanner::detailScrollOffset = 0;
unsigned long BLEScanner::lastScanTime = 0;
unsigned long BLEScanner::lastDisplayUpdate = 0;
BLEScan* BLEScanner::pBLEScan = nullptr;

void BLEScanner::init() {
    BLEDevice::init("Starbeam");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);   // Send scan requests
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
    deviceCount = 0;
    currentDeviceIndex = 0;
    detailMode = false;
    detailScrollOffset = 0;
    lastScanTime = 0;
}

void BLEScanner::deinit() {
    // Free all payload buffers
    for (int i = 0; i < deviceCount; i++) {
        if (devices[i].payload != nullptr) {
            delete[] devices[i].payload;
            devices[i].payload = nullptr;
        }
    }
    BLEDevice::deinit();
}

const BLEDeviceInfo* BLEScanner::getDevice(int index) {
    if (index >= 0 && index < deviceCount) {
        return &devices[index];
    }
    return nullptr;
}

void BLEScanner::runScanner() {
    performScan();  // Initial scan

    // Display initial results
    if (detailMode) {
        displayDetailMode();
    } else {
        displayListMode();
    }

    while (true) {
        // Periodic rescanning
        if (millis() - lastScanTime >= BLE_SCAN_INTERVAL) {
            performScan();
            // Display updated results after rescan
            if (detailMode) {
                displayDetailMode();
            } else {
                displayListMode();
            }
        }

        // Button handling
        if (Input::isButtonPressed(BUTTON_UP)) {
            if (detailMode) {
                detailScrollOffset = max(0, detailScrollOffset - 1);
            } else {
                if (deviceCount > 0) {
                    currentDeviceIndex = (currentDeviceIndex - 1 + deviceCount) % deviceCount;
                }
            }
            // Update display on navigation
            if (detailMode) {
                displayDetailMode();
            } else {
                displayListMode();
            }
            delay(150);
        }

        if (Input::isButtonPressed(BUTTON_DOWN)) {
            if (detailMode) {
                detailScrollOffset++;  // Will be clamped in display function
            } else {
                if (deviceCount > 0) {
                    currentDeviceIndex = (currentDeviceIndex + 1) % deviceCount;
                }
            }
            // Update display on navigation
            if (detailMode) {
                displayDetailMode();
            } else {
                displayListMode();
            }
            delay(150);
        }

        if (Input::isButtonPressed(BUTTON_SELECT)) {
            // Check for long press (2s) to exit
            unsigned long pressStart = millis();
            while (Input::isButtonPressed(BUTTON_SELECT)) {
                if (millis() - pressStart > 2000) {
                    return;  // Exit to menu
                }
                yield();
            }

            // Only toggle if we have devices
            if (deviceCount > 0) {
                detailMode = !detailMode;
                detailScrollOffset = 0;
                // Update display on mode toggle
                if (detailMode) {
                    displayDetailMode();
                } else {
                    displayListMode();
                }
            }
            delay(200);
        }

        // Keep handleClient() call for compatibility
        if (StarbeamWebServer::isRunning()) {
            StarbeamWebServer::handleClient();
        }

        // Terminal stop command
        if (Terminal::stopRequested()) {
            Terminal::clearStopFlag();
            Serial.println("BLE scan stopped via terminal");
            return;
        }

        yield();
    }
}

void BLEScanner::performScan() {
    // Clear old devices
    for (int i = 0; i < deviceCount; i++) {
        if (devices[i].payload != nullptr) {
            delete[] devices[i].payload;
            devices[i].payload = nullptr;
        }
    }
    deviceCount = 0;

    BLEScanResults* scanResults = pBLEScan->start(BLE_SCAN_DURATION, false);
    int count = scanResults->getCount();

    for (int i = 0; i < count && deviceCount < MAX_BLE_DEVICES; i++) {
        BLEAdvertisedDevice device = scanResults->getDevice(i);
        parseAdvertisingData(device, devices[deviceCount]);
        deviceCount++;
    }

    pBLEScan->clearResults();
    lastScanTime = millis();

    // Clamp current index
    if (currentDeviceIndex >= deviceCount && deviceCount > 0) {
        currentDeviceIndex = deviceCount - 1;
    }
}

void BLEScanner::parseAdvertisingData(BLEAdvertisedDevice device, BLEDeviceInfo& info) {
    // Extract name
    if (device.haveName()) {
        strncpy(info.name, device.getName().c_str(), sizeof(info.name) - 1);
        info.name[sizeof(info.name) - 1] = '\0';
    } else {
        strcpy(info.name, "Unknown");
    }

    // Extract MAC address
    strncpy(info.address, device.getAddress().toString().c_str(), sizeof(info.address) - 1);
    info.address[sizeof(info.address) - 1] = '\0';

    // Extract RSSI
    info.rssi = device.getRSSI();

    // Extract address type
    info.addressType = device.getAddressType();

    // Get raw payload
    uint8_t* payload = device.getPayload();
    size_t length = device.getPayloadLength();

    info.payloadLength = length;
    info.payload = new uint8_t[length];
    memcpy(info.payload, payload, length);

    // Parse flags byte for connectable bit
    info.connectable = false;
    for (size_t i = 0; i < length; ) {
        uint8_t len = payload[i];
        if (len == 0 || i + len >= length) break;

        uint8_t type = payload[i + 1];
        if (type == 0x01) {  // Flags
            uint8_t flags = payload[i + 2];
            info.connectable = (flags & 0x02) != 0;  // Bit 1 = General Discoverable
            break;
        }
        i += len + 1;
    }
}

void BLEScanner::displayListMode() {
    Adafruit_SSD1306& oled = Display::getOled();
    oled.clearDisplay();

    // Title with device count
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.printf("BLE Scanner (%d)", deviceCount);

    // Separator
    oled.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

    if (deviceCount == 0) {
        oled.setCursor(0, 20);
        oled.print("No devices found");
        oled.display();
        return;
    }

    BLEDeviceInfo& dev = devices[currentDeviceIndex];

    // Cursor indicator
    oled.setCursor(0, 15);
    oled.print("[>] ");
    oled.println(dev.name);

    // MAC address
    oled.setCursor(4, 25);
    oled.print("MAC: ");
    oled.println(dev.address);

    // RSSI
    oled.setCursor(4, 35);
    oled.printf("RSSI: %d dBm", dev.rssi);

    // Footer
    oled.setCursor(0, 55);
    oled.print("[SELECT] Detail");

    oled.display();
}

void BLEScanner::displayDetailMode() {
    Adafruit_SSD1306& oled = Display::getOled();
    oled.clearDisplay();

    if (deviceCount == 0) {
        oled.setCursor(0, 20);
        oled.print("No device selected");
        oled.display();
        return;
    }

    BLEDeviceInfo& dev = devices[currentDeviceIndex];

    // Build detail lines
    String lines[20];  // Max 20 lines
    int lineCount = 0;

    lines[lineCount++] = String("[") + dev.name + "]";
    lines[lineCount++] = "MAC: " + String(dev.address);
    lines[lineCount++] = "RSSI: " + String(dev.rssi) + " dBm " +
                         (dev.addressType == 0 ? "Pub" : "Rnd");
    lines[lineCount++] = dev.connectable ? "Connectable" : "Non-connectable";
    lines[lineCount++] = "---";

    // Extract service UUIDs
    char services[64];
    extractServiceUUIDs(dev.payload, dev.payloadLength, services, sizeof(services));
    if (strlen(services) > 0) {
        lines[lineCount++] = "Svc: " + String(services);
    }

    // Extract manufacturer data
    char mfr[64];
    extractManufacturerData(dev.payload, dev.payloadLength, mfr, sizeof(mfr));
    if (strlen(mfr) > 0) {
        lines[lineCount++] = "Mfr: " + String(mfr);
    }

    // Raw hex dump (first 40 bytes)
    lines[lineCount++] = "Raw:";
    String hexLine = "";
    int bytesToShow = min((int)dev.payloadLength, 40);
    for (int i = 0; i < bytesToShow; i++) {
        char hex[4];
        sprintf(hex, "%02X ", dev.payload[i]);
        hexLine += hex;
        if (hexLine.length() > 18) {  // ~18 chars per line
            lines[lineCount++] = hexLine;
            hexLine = "";
        }
    }
    if (hexLine.length() > 0) {
        if (dev.payloadLength > 40) hexLine += "...";
        lines[lineCount++] = hexLine;
    }

    // Clamp scroll offset
    int maxScroll = max(0, lineCount - 7);  // 7 lines visible
    detailScrollOffset = constrain(detailScrollOffset, 0, maxScroll);

    // Display visible lines
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    int y = 0;
    for (int i = detailScrollOffset; i < min(lineCount, detailScrollOffset + 7); i++) {
        oled.setCursor(0, y);
        oled.println(lines[i]);
        y += 9;
    }

    // Footer
    oled.setCursor(0, 55);
    oled.print("[SELECT] Back");

    oled.display();
}

void BLEScanner::listAll() {
    // Initialize BLE
    init();

    // Perform scan
    Serial.println("Scanning BLE devices...");
    performScan();

    // Print header
    Serial.println("\n========================================");
    Serial.printf("Found %d BLE devices:\n", deviceCount);
    Serial.println("========================================");

    if (deviceCount == 0) {
        Serial.println("No devices found");
    } else {
        // Print each device - compact format
        for (int i = 0; i < deviceCount; i++) {
            BLEDeviceInfo& dev = devices[i];

            String name = String(dev.name);
            if (name.length() == 0 || name == "Unknown") {
                name = "[Unknown]";
            }
            // Truncate long names
            if (name.length() > 14) {
                name = name.substring(0, 13) + "~";
            }

            const char* addrType = (dev.addressType == 0) ? "P" : "R";
            const char* connType = dev.connectable ? "C" : "N";

            // Extract manufacturer if available
            char mfr[32];
            extractManufacturerData(dev.payload, dev.payloadLength, mfr, sizeof(mfr));

            // Format: #. Name MAC RSSI T C [Mfr]
            Serial.printf("%2d. %-14s %s %4ddBm %s%s",
                i + 1,
                name.c_str(),
                dev.address,
                dev.rssi,
                addrType,
                connType);

            // Add manufacturer on same line if present
            if (strlen(mfr) > 0) {
                Serial.printf(" %s", mfr);
            }
            Serial.println();
        }
    }

    Serial.println("========================================\n");

    // Cleanup
    deinit();
}

void BLEScanner::extractServiceUUIDs(uint8_t* payload, size_t length, char* output, size_t outputSize) {
    output[0] = '\0';
    String result = "";

    for (size_t i = 0; i < length; ) {
        uint8_t len = payload[i];
        if (len == 0 || i + len >= length) break;

        uint8_t type = payload[i + 1];

        // 16-bit UUIDs (types 0x02, 0x03)
        if (type == 0x02 || type == 0x03) {
            for (int j = 0; j < (len - 1) / 2; j++) {
                uint16_t uuid = payload[i + 2 + j*2] | (payload[i + 3 + j*2] << 8);
                if (result.length() > 0) result += ",";
                result += "0x" + String(uuid, HEX);
            }
        }

        i += len + 1;
    }

    strncpy(output, result.c_str(), outputSize - 1);
    output[outputSize - 1] = '\0';
}

void BLEScanner::extractManufacturerData(uint8_t* payload, size_t length, char* output, size_t outputSize) {
    output[0] = '\0';

    for (size_t i = 0; i < length; ) {
        uint8_t len = payload[i];
        if (len == 0 || i + len >= length) break;

        uint8_t type = payload[i + 1];

        if (type == 0xFF) {  // Manufacturer data
            if (i + 3 < length) {
                uint16_t companyId = payload[i + 2] | (payload[i + 3] << 8);
                snprintf(output, outputSize, "0x%04X", companyId);

                // Add company name if known
                if (companyId == 0x004C) strcat(output, " (Apple)");
                else if (companyId == 0x0006) strcat(output, " (Microsoft)");
                else if (companyId == 0x00E0) strcat(output, " (Google)");
            }
            break;
        }

        i += len + 1;
    }
}
