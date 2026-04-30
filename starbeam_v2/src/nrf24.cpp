// Project Starbeam V2 - NRF24 Radio Module Implementation
// Ported from V1

#include "nrf24.h"
#include "display.h"
#include "../config.h"

// Static member initialization
SPIClass NRF24Radio::vspi(VSPI);
SPIClass NRF24Radio::hspi(HSPI);
RF24 NRF24Radio::radio1(NRF24_1_CE, NRF24_1_CS, 16000000);
RF24 NRF24Radio::radio2(NRF24_2_CE, NRF24_2_CS, 16000000);
RF24 NRF24Radio::radio3(NRF24_3_CE, NRF24_3_CS, 16000000);
RF24 NRF24Radio::radio4(NRF24_4_CE, NRF24_4_CS, 16000000);
RF24 NRF24Radio::radio5(NRF24_5_CE, NRF24_5_CS, 16000000);
bool NRF24Radio::initialized = false;

// Helper function to initialize a single radio
bool NRF24Radio::initializeRadio(RF24 &radio, SPIClass *spi, const char *name) {
    if (radio.begin(spi)) {
        Serial.printf("%s Started\n", name);
        radio.setAutoAck(false);
        radio.stopListening();
        radio.setRetries(0, 0);
        radio.setPALevel(RF24_PA_MAX, true);
        radio.setDataRate(RF24_2MBPS);
        radio.setCRCLength(RF24_CRC_DISABLED);
        radio.printPrettyDetails();
        radio.startConstCarrier(RF24_PA_MAX, 45);
        return true;
    } else {
        Serial.printf("%s Failed to Start\n", name);
        return false;
    }
}

// Initialize top 3 radios on VSPI bus
void NRF24Radio::initVSPI() {
    Adafruit_SSD1306& oled = Display::getOled();
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.println("Initializing VSPI...");
    oled.display();

    vspi.begin();
    pinMode(NRF24_2_CS, INPUT_PULLUP);
    pinMode(NRF24_2_CE, OUTPUT);

    oled.setCursor(0, 10);
    oled.println("Init Radio 1...");
    oled.display();
    bool r1 = initializeRadio(radio1, &vspi, "Radio 1");
    delay(500);

    oled.setCursor(0, 20);
    oled.println("Init Radio 2...");
    oled.display();
    bool r2 = initializeRadio(radio2, &vspi, "Radio 2");
    delay(500);

    oled.setCursor(0, 30);
    oled.println("Init Radio 3...");
    oled.display();
    bool r3 = initializeRadio(radio3, &vspi, "Radio 3");
    delay(500);

    oled.setCursor(0, 40);
    if (r1 && r2 && r3) {
        oled.println("VSPI: All OK!");
    } else {
        oled.println("VSPI: Some failed!");
    }
    oled.display();

    initialized = true;
}

// Initialize side 2 radios on HSPI bus
void NRF24Radio::initHSPI() {
    Adafruit_SSD1306& oled = Display::getOled();
    oled.setCursor(0, 50);
    oled.println("Init HSPI...");
    oled.display();

    hspi.begin();

    bool r4 = initializeRadio(radio4, &hspi, "Radio 4");
    delay(500);
    bool r5 = initializeRadio(radio5, &hspi, "Radio 5");
    delay(500);

    oled.setCursor(0, 56);
    if (r4 && r5) {
        oled.println("HSPI: All OK!");
    } else {
        oled.println("HSPI: Some failed!");
    }
    oled.display();
}

// Bluetooth jamming - random channels 0-81
void NRF24Radio::btJam() {
    radio1.setChannel(random(81));
    radio2.setChannel(random(81));
    radio3.setChannel(random(81));
    radio4.setChannel(random(81));
    radio5.setChannel(random(81));
    delayMicroseconds(random(60));
}

// Drone jamming - random channels 0-126 (full 2.4GHz range)
void NRF24Radio::droneJam() {
    radio1.setChannel(random(126));
    radio2.setChannel(random(126));
    radio3.setChannel(random(126));
    radio4.setChannel(random(126));
    radio5.setChannel(random(126));
    delayMicroseconds(random(60));
}

// WiFi jamming - channels 1, 6, 14
void NRF24Radio::wifiJam() {
    int channels[] = {1, 6, 14};
    int randomIndex = random(3);
    int channel = channels[randomIndex];

    radio1.setChannel(channel);
    radio2.setChannel(channel);
    radio3.setChannel(channel);
    radio4.setChannel(channel);
    radio5.setChannel(channel);

    Serial.printf("WiFi jam channel: %d\n", channel);
}

// Single channel mode - mixed ranges
void NRF24Radio::singleChannel() {
    radio1.setChannel(random(15));
    radio2.setChannel(random(81));
    radio3.setChannel(random(15));
    radio4.setChannel(random(81));
    radio5.setChannel(random(81));
    delayMicroseconds(random(60));
}

// Channel range mode - 40-80 range
void NRF24Radio::channelRange() {
    int channel = random(40, 81);
    radio1.setChannel(channel);
    radio2.setChannel(channel);
    radio3.setChannel(channel);
    radio4.setChannel(channel);
    radio5.setChannel(channel);
    Serial.printf("Channel range: %d\n", channel);
}

bool NRF24Radio::isInitialized() {
    return initialized;
}
