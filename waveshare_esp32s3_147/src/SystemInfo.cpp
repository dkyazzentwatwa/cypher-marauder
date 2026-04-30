#include "SystemInfo.h"

#include <WiFi.h>

void drawSystemInfo(DisplayPort &display) {
  String body;
  body += "Chip rev: ";
  body += ESP.getChipRevision();
  body += "\nCPU MHz: ";
  body += ESP.getCpuFreqMHz();
  body += "\nFlash: ";
  body += ESP.getFlashChipSize() / 1024 / 1024;
  body += " MB\nPSRAM: ";
  body += ESP.getPsramSize() / 1024 / 1024;
  body += " MB\nHeap: ";
  body += ESP.getFreeHeap();
  body += "\nSDK: ";
  body += ESP.getSdkVersion();
  body += "\nMAC: ";
  body += WiFi.macAddress();
  display.drawMessage("System Info", body, "Click/5s back");
}

void drawAbout(DisplayPort &display) {
  String body;
  body += "Waveshare 1.47\n";
  body += "172x320 JD9853\n";
  body += "Touch AXS5106L\n";
  body += "Click: next/back\n";
  body += "Hold 2s: select\n";
  body += "Hold 5s: system/back\n";
  body += "Verify LCD_RST pin";
  display.drawMessage("About", body, "Click/5s back");
}
