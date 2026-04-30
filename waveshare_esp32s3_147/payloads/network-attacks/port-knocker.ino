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

  // Port knock sequence trigger
  // Opens firewall rule on sequence
  // Author: Generated
  // Title: Port Knocker
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

  Keyboard.print("for port in 1000 2000 3000; do nc -z -w1 TARGET_IP $port; done");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo 'Port knock sequence sent'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}