#ifndef BLE_SCANNER_H
#define BLE_SCANNER_H

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Constants
#define MAX_BLE_DEVICES 20
#define BLE_SCAN_DURATION 5
#define BLE_SCAN_INTERVAL 10000
#define DISPLAY_UPDATE_INTERVAL 1000 // Display update throttle (1 second)

// Data structure
struct BLEDeviceInfo {
    char name[32];
    char address[18];
    int rssi;
    uint8_t* payload;
    size_t payloadLength;
    uint8_t addressType;
    bool connectable;
};

class BLEScanner {
public:
    static void init();
    static void deinit();
    static void runScanner();

    // Terminal list mode (show all devices)
    static void listAll();

    // Web server data access
    static int getDeviceCount() { return deviceCount; }
    static const BLEDeviceInfo* getDevice(int index);
    static bool isScanning() { return (pBLEScan != nullptr); }
    static bool isInitialized() { return (pBLEScan != nullptr); }

    // Background scanning (for WEBSERVER mode)
    static void performScan();

private:
    static void parseAdvertisingData(BLEAdvertisedDevice device, BLEDeviceInfo& info);
    static void displayListMode();
    static void displayDetailMode();
    static void extractServiceUUIDs(uint8_t* payload, size_t length, char* output, size_t outputSize);
    static void extractManufacturerData(uint8_t* payload, size_t length, char* output, size_t outputSize);

    // State variables
    static BLEDeviceInfo devices[MAX_BLE_DEVICES];
    static int deviceCount;
    static int currentDeviceIndex;
    static bool detailMode;
    static int detailScrollOffset;
    static unsigned long lastScanTime;
    static unsigned long lastDisplayUpdate;
    static BLEScan* pBLEScan;
};

#endif
