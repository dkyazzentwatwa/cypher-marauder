#include "DevkitSerial.h"

#include <WiFi.h>
#include "MarauderCore.h"

namespace {
constexpr int kBootButtonPin = 0;
constexpr int kLedPin = 2;
constexpr uint32_t kStatusIntervalMs = 5000;

uint32_t lastStatusMs = 0;
bool ledState = false;
}  // namespace

void DevkitSerial::begin(uint32_t baud) {
  Serial.begin(baud);
  delay(200);

  pinMode(kBootButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(100);

  MarauderCore::core().begin("esp32-devkitc");
  printBanner();
  printHelp();
}

void DevkitSerial::update() {
  while (Serial.available() > 0) {
    char c = static_cast<char>(Serial.read());
    if (c == '\r') {
      continue;
    }
    if (c == '\n') {
      String command = input_;
      input_ = "";
      command.trim();
      if (command.length() > 0) {
        handleCommand(command);
      }
      Serial.print(F("> "));
    } else if (isPrintable(c)) {
      input_ += c;
    }
  }

  if (millis() - lastStatusMs >= kStatusIntervalMs) {
    lastStatusMs = millis();
    Serial.printf("[alive] uptime=%lus heap=%u boot=%s\n",
                  millis() / 1000,
                  ESP.getFreeHeap(),
                  digitalRead(kBootButtonPin) == LOW ? "pressed" : "released");
  }

  MarauderCore::core().poll();
}

void DevkitSerial::printBanner() {
  Serial.println();
  Serial.println(F("Cypher Marauder - ESP32 DevKitC"));
  Serial.println(F("Serial-only beginner firmware, no external modules."));
  Serial.println(F("Baud: 115200"));
  Serial.print(F("> "));
}

void DevkitSerial::printHelp() {
  Serial.println(F("Commands:"));
  Serial.println(F("  help       Show commands"));
  Serial.println(F("  status     Print board status"));
  Serial.println(F("  wifi_scan  Scan nearby WiFi networks"));
  Serial.println(F("  marauder help/status/wifi/monitor/stop"));
  Serial.println(F("  led on     Turn GPIO2 LED on"));
  Serial.println(F("  led off    Turn GPIO2 LED off"));
  Serial.println(F("  restart    Reboot the board"));
}

void DevkitSerial::printStatus() {
  Serial.printf("Chip: %s rev %u\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("Cores: %u\n", ESP.getChipCores());
  Serial.printf("Flash: %u bytes\n", ESP.getFlashChipSize());
  Serial.printf("Heap: %u bytes\n", ESP.getFreeHeap());
  Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
  Serial.printf("Boot button: %s\n", digitalRead(kBootButtonPin) == LOW ? "pressed" : "released");
  Serial.printf("LED GPIO2: %s\n", ledState ? "on" : "off");
}

void DevkitSerial::scanWifi() {
  Serial.println(F("Scanning WiFi..."));
  int count = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  if (count <= 0) {
    Serial.println(F("No networks found."));
    return;
  }

  for (int i = 0; i < count; i++) {
    Serial.printf("%2d  ch=%2d  rssi=%4d  %s  %s\n",
                  i + 1,
                  WiFi.channel(i),
                  WiFi.RSSI(i),
                  WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open " : "secure",
                  WiFi.SSID(i).c_str());
  }
  WiFi.scanDelete();
}

void DevkitSerial::setLed(bool on) {
  ledState = on;
  digitalWrite(kLedPin, on ? HIGH : LOW);
  Serial.printf("LED %s\n", on ? "on" : "off");
}

void DevkitSerial::handleCommand(String command) {
  command.toLowerCase();

  if (MarauderCore::core().handleCommand(command)) {
    return;
  }

  if (command == "help") {
    printHelp();
  } else if (command == "status") {
    printStatus();
  } else if (command == "wifi_scan" || command == "scan") {
    scanWifi();
  } else if (command == "led on") {
    setLed(true);
  } else if (command == "led off") {
    setLed(false);
  } else if (command == "restart" || command == "reboot") {
    Serial.println(F("Restarting..."));
    delay(100);
    ESP.restart();
  } else {
    Serial.println(F("Unknown command. Type help."));
  }
}
