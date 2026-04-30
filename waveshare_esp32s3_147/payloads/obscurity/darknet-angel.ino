// Generated with <3 by Dckuino.js !

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

  // Code Begins Here

  // spoofs the MAC address
  // randomizes DNS requests
  // enables firewall stealth mode
  // and engages the cloaking device
  // 7 seconds to enter password
  // Open the Terminal application on your macOS.
  // Run the following command: sudo mkdir /etc/resolver
  // This will create the /etc/resolver directory needed to randomize DNS request.
  // Author: Narsty
  // Title: DarkNet Angel
  // Target: MacOS
  // Version: 1.0
  // Category: Obscurity
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

  Keyboard.print("sudo ifconfig en0 ether 00:11:22:33:44:55");

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.print("sudo sh -c 'echo \"nameserver 127.0.0.1\" > /etc/resolver/local'");

  typeKey(KEY_RETURN);

  delay(2000);

  Keyboard.print("sudo defaults write /Library/Preferences/com.apple.alf stealthenabled -bool true");

  typeKey(KEY_RETURN);

  delay(2000);

  Keyboard.print("sudo sysctl -w net.inet.tcp.rfc6298valtso=0");

  typeKey(KEY_RETURN);

  delay(2000);

  Keyboard.print("exit");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}