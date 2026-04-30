#pragma once

#include <Arduino.h>

class DevkitSerial {
 public:
  void begin(uint32_t baud);
  void update();

 private:
  String input_;

  void printBanner();
  void printHelp();
  void printStatus();
  void scanWifi();
  void setLed(bool on);
  void handleCommand(String command);
};
