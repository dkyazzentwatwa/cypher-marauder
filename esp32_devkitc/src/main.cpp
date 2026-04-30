#include <Arduino.h>

#include "DevkitSerial.h"

DevkitSerial devkitSerial;

void setup() {
  devkitSerial.begin(115200);
}

void loop() {
  devkitSerial.update();
  delay(10);
}
