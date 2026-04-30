// Project Starbeam V2 - NRF24 Radio Module
// Ported from V1 for modular architecture

#ifndef NRF24_H
#define NRF24_H

#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"
#include "../config.h"

class NRF24Radio {
public:
    // Initialization
    static void initVSPI();      // Initialize radios 1-3 on VSPI bus
    static void initHSPI();      // Initialize radios 4-5 on HSPI bus

    // Jamming functions
    static void btJam();         // Bluetooth jamming (channels 0-81)
    static void wifiJam();       // WiFi jamming (channels 1, 6, 14)
    static void droneJam();      // Drone jamming (channels 0-126)
    static void singleChannel(); // Single channel mode
    static void channelRange();  // Channel range mode

    // Status
    static bool isInitialized();

private:
    static SPIClass vspi;
    static SPIClass hspi;
    static RF24 radio1;
    static RF24 radio2;
    static RF24 radio3;
    static RF24 radio4;
    static RF24 radio5;
    static bool initialized;

    static bool initializeRadio(RF24 &radio, SPIClass *spi, const char *name);
};

#endif // NRF24_H
