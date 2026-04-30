// Project Starbeam V2 - Recording Module
// Ported from V1 for modular architecture

#ifndef RECORDING_H
#define RECORDING_H

#include <Arduino.h>
#include <EEPROM.h>
#include "../config.h"

class Recording {
public:
    // Initialization
    static void init();

    // Recording operations
    static void recordRawData(int interval);
    static void playRawData(int interval);
    static void showRawData();
    static void showBitData();
    static void flushBuffer();

    // Frame-based operations
    static void toggleRecordingMode();
    static void playRecordedFrames(int frameNumber);
    static void addFrame(const char* hexData);
    static void showRecordedFrames();

    // Persistence
    static void saveToEEPROM();
    static void loadFromEEPROM();

    // Status
    static int getFrameCount() { return framesInBuffer; }
    static int getBufferPosition() { return bufferPosition; }
    static bool isRecording() { return recordingMode; }

private:
    // Buffers
    static byte recordingBuffer[RECORDINGBUFFERSIZE];
    static byte receiveBuffer[CCBUFFERSIZE];
    static byte sendBuffer[CCBUFFERSIZE];
    static byte textBuffer[BUF_LENGTH];

    // State
    static int bufferPosition;
    static int framesInBuffer;
    static bool recordingMode;

    // Helper functions
    static void asciiToHex(byte* ascii, byte* hex, int len);
    static void hexToAscii(byte* ascii, byte* hex, int len);
};

#endif // RECORDING_H
