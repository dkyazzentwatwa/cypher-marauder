// Project Starbeam V2 - Recording Module Implementation
// Ported from V1

#include "recording.h"
#include "cc1101.h"
#include "display.h"
#include <Adafruit_SSD1306.h>

// Static member initialization
byte Recording::recordingBuffer[RECORDINGBUFFERSIZE] = {0};
byte Recording::receiveBuffer[CCBUFFERSIZE] = {0};
byte Recording::sendBuffer[CCBUFFERSIZE] = {0};
byte Recording::textBuffer[BUF_LENGTH] = {0};
int Recording::bufferPosition = 0;
int Recording::framesInBuffer = 0;
bool Recording::recordingMode = false;

void Recording::init() {
    EEPROM.begin(EEPROM_SIZE);
    flushBuffer();
    Serial.println("Recording module initialized");
}

// Convert bytes to hex string
void Recording::asciiToHex(byte* ascii, byte* hex, int len) {
    byte i, j, k;
    for (i = 0; i < len; i++) {
        j = ascii[i] / 16;
        if (j > 9) {
            k = j - 10 + 65;
        } else {
            k = j + 48;
        }
        hex[2 * i] = k;

        j = ascii[i] % 16;
        if (j > 9) {
            k = j - 10 + 65;
        } else {
            k = j + 48;
        }
        hex[(2 * i) + 1] = k;
    }
    hex[(2 * i) + 2] = '\0';
}

// Convert hex string to bytes
void Recording::hexToAscii(byte* ascii, byte* hex, int len) {
    byte i, j;
    for (i = 0; i < (len / 2); i++) {
        j = hex[i * 2];
        if ((j > 47) && (j < 58))
            ascii[i] = (j - 48) * 16;
        if ((j > 64) && (j < 71))
            ascii[i] = (j - 55) * 16;
        if ((j > 96) && (j < 103))
            ascii[i] = (j - 87) * 16;

        j = hex[i * 2 + 1];
        if ((j > 47) && (j < 58))
            ascii[i] = ascii[i] + (j - 48);
        if ((j > 64) && (j < 71))
            ascii[i] = ascii[i] + (j - 55);
        if ((j > 96) && (j < 103))
            ascii[i] = ascii[i] + (j - 87);
    }
    ascii[i++] = '\0';
}

// Record raw RF data
void Recording::recordRawData(int interval) {
    if (interval <= 0) {
        Serial.println("Invalid interval");
        return;
    }

    ELECHOUSE_CC1101& CC1 = CC1101Radio::getCC1();

    CC1.setCCMode(0);
    CC1.setPktFormat(3);
    CC1.SetRx();

    Display::displayInfo("Recording", "Waiting for", "signal...", "");
    Serial.println("Waiting for signal to start recording...");

    pinMode(CC1101_1_GDO0, INPUT);
    while (digitalRead(CC1101_1_GDO0) == LOW) {
        yield();
    }

    Display::displayInfo("Recording", "Recording RAW", "data...", "");
    Serial.println("Recording RAW data...");

    for (int i = 0; i < RECORDINGBUFFERSIZE; i++) {
        byte receivedByte = 0;
        for (int j = 7; j >= 0; j--) {
            bitWrite(receivedByte, j, digitalRead(CC1101_1_GDO0));
            delayMicroseconds(interval);
        }
        recordingBuffer[i] = receivedByte;
    }

    Display::displayInfo("Recording", "Complete!", "", "");
    Serial.println("Recording complete");
}

// Playback raw RF data
void Recording::playRawData(int interval) {
    if (interval <= 0) {
        Serial.println("Invalid interval");
        return;
    }

    ELECHOUSE_CC1101& CC1 = CC1101Radio::getCC1();

    CC1.setCCMode(0);
    CC1.setPktFormat(3);
    CC1.SetTx();

    Display::displayInfo("Playback", "Replaying RAW", "data...", "");
    Serial.println("Replaying RAW data...");

    pinMode(CC1101_1_GDO0, OUTPUT);
    for (int i = 1; i < RECORDINGBUFFERSIZE; i++) {
        byte receivedByte = recordingBuffer[i];
        for (int j = 7; j >= 0; j--) {
            digitalWrite(CC1101_1_GDO0, bitRead(receivedByte, j));
            delayMicroseconds(interval);
        }
    }

    Display::displayInfo("Playback", "Complete!", "", "");
    Serial.println("Playback complete");
}

// Display raw data as hex
void Recording::showRawData() {
    Adafruit_SSD1306& oled = Display::getOled();

    Serial.println("Recorded RAW data:");
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.println("RAW Data:");

    int y = 10;
    for (int i = 0; i < RECORDINGBUFFERSIZE; i += 32) {
        asciiToHex(&recordingBuffer[i], textBuffer, 32);
        Serial.print((char*)textBuffer);
        oled.setCursor(0, y);
        oled.print((char*)textBuffer);
        y += 10;
        if (y > 50) break;
    }
    oled.display();
    Serial.println();
}

// Display raw data as bit stream
void Recording::showBitData() {
    Adafruit_SSD1306& oled = Display::getOled();

    Serial.println("Recorded RAW data as bit stream:");
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.println("Bit Stream:");

    int y = 10;
    for (int i = 0; i < RECORDINGBUFFERSIZE; i += 32) {
        asciiToHex(&recordingBuffer[i], textBuffer, 32);
        String bitStream = "";
        for (int s = 0; s < 64; s++) {
            char c = textBuffer[s];
            switch (c) {
                case '0': bitStream += "____"; break;
                case '1': bitStream += "___-"; break;
                case '2': bitStream += "__-_"; break;
                case '3': bitStream += "__--"; break;
                case '4': bitStream += "_-__"; break;
                case '5': bitStream += "_-_-"; break;
                case '6': bitStream += "_--_"; break;
                case '7': bitStream += "_---"; break;
                case '8': bitStream += "-___"; break;
                case '9': bitStream += "-__-"; break;
                case 'A': bitStream += "-_-_"; break;
                case 'B': bitStream += "-_--"; break;
                case 'C': bitStream += "--__"; break;
                case 'D': bitStream += "--_-"; break;
                case 'E': bitStream += "---_"; break;
                case 'F': bitStream += "----"; break;
            }
        }
        Serial.print(bitStream);
        oled.setCursor(0, y);
        oled.print(bitStream.substring(0, 21));
        y += 10;
        if (y > 50) break;
    }
    oled.display();
    Serial.println();
}

void Recording::flushBuffer() {
    for (int i = 0; i < RECORDINGBUFFERSIZE; i++) {
        recordingBuffer[i] = 0;
    }
    bufferPosition = 0;
    framesInBuffer = 0;
    Serial.println("Recording buffer cleared");
}

void Recording::toggleRecordingMode() {
    if (recordingMode) {
        recordingMode = false;
        bufferPosition = 0;
        Serial.println("Recording mode disabled");
    } else {
        CC1101Radio::getCC1().SetRx();
        bufferPosition = 0;
        flushBuffer();
        recordingMode = true;
        framesInBuffer = 0;
        Serial.println("Recording mode enabled");
    }
}

void Recording::playRecordedFrames(int frameNumber) {
    if (frameNumber > framesInBuffer) {
        Serial.println("Invalid frame number");
        return;
    }

    Serial.println("Replaying recorded frames...");
    bufferPosition = 0;

    if (framesInBuffer > 0) {
        for (int i = 1; i <= framesInBuffer; i++) {
            int len = recordingBuffer[bufferPosition];
            if ((len <= 60) && (len > 0) && ((i == frameNumber) || (frameNumber == 0))) {
                memcpy(sendBuffer, &recordingBuffer[bufferPosition + 1], len);
                CC1101Radio::getCC1().SendData(sendBuffer, (byte)len);
            }
            bufferPosition = bufferPosition + 1 + len;
            if (bufferPosition > RECORDINGBUFFERSIZE) break;
        }
    }
    bufferPosition = 0;
    Serial.println("Playback done");
}

void Recording::addFrame(const char* hexData) {
    int len = strlen(hexData);
    if ((len > 120) || (len <= 0)) {
        Serial.println("Invalid data length");
        return;
    }

    hexToAscii(textBuffer, (byte*)hexData, len);
    len = len / 2;

    if ((bufferPosition + len + 1) < RECORDINGBUFFERSIZE) {
        recordingBuffer[bufferPosition] = len;
        bufferPosition++;
        memcpy(&recordingBuffer[bufferPosition], &textBuffer, len);
        bufferPosition += len;
        framesInBuffer++;
        Serial.printf("Added frame %d\n", framesInBuffer);
    } else {
        Serial.println("Buffer full");
    }
}

void Recording::showRecordedFrames() {
    if (framesInBuffer <= 0) {
        Serial.println("No frames recorded");
        return;
    }

    Serial.println("Recorded frames:");
    int pos = 0;

    for (int i = 1; i <= framesInBuffer; i++) {
        int len = recordingBuffer[pos];
        if ((len <= 60) && (len > 0)) {
            for (int j = 0; j < BUF_LENGTH; j++) {
                textBuffer[j] = 0;
            }
            asciiToHex(&recordingBuffer[pos + 1], textBuffer, len);
            Serial.printf("Frame %d: %s\n", i, (char*)textBuffer);
        }
        pos = pos + 1 + len;
        if (pos > RECORDINGBUFFERSIZE) break;
    }
}

void Recording::saveToEEPROM() {
    Serial.println("Saving to EEPROM...");
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, recordingBuffer[i]);
    }
    EEPROM.commit();
    Serial.println("Save complete");
}

void Recording::loadFromEEPROM() {
    flushBuffer();
    Serial.println("Loading from EEPROM...");
    for (int i = 0; i < EEPROM_SIZE; i++) {
        recordingBuffer[i] = EEPROM.read(i);
    }
    Serial.println("Load complete");
}
