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

  // Restores the original MAC address.
  // removes the DNS configuration.
  // disables firewall stealth mode.
  // and disengages the cloaking device.
  // 7 seconds to enter password
  // Run ifconfig en0 in the terminal to get your original MAC address
  // It will be displayed in the format xx:xx:xx:xx:xx:xx.
  // Replace <original_mac_address> in the script with the actual MAC address you obtained.
  // Author: Narsty
  // Title: DarkNet Angel Restore
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

  delay(500);

  Keyboard.print("sudo ifconfig en0 ether Original_mac_address");

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.print("sudo rm /etc/resolver/local");

  typeKey(KEY_RETURN);

  delay(2000);

  Keyboard.print("sudo defaults write /Library/Preferences/com.apple.alf stealthenabled -bool false");

  typeKey(KEY_RETURN);

  delay(2000);

  Keyboard.print("sudo sysctl -w net.inet.tcp.rfc6298valtso=1");

  typeKey(KEY_RETURN);

  delay(2000);

  Keyboard.print("exit");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}