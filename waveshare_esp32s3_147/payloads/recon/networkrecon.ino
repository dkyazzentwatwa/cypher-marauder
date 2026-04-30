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

  // Use responsibly and with proper permissions.
  // This script initiates a network reconnaissance by scanning for live hosts,
  // performing a port scan on live hosts,
  // Author: Narsty
  // Title: Network Reconnaissance
  // Target: MacOS
  // Version: 1.0
  // Category: Recon
  // Replace (15.151.298.31) with your targets I.P. address
  // can insert multiple I.p.'s if needed
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

  Keyboard.print("echo \"Network Reconnaissance:\"");

  Keyboard.print("&& echo \"------------------------------\"");

  Keyboard.print("&& echo \"Scan Date: $(date)\"");

  Keyboard.print("&& echo \"------------------------------\"");

  Keyboard.print("&& echo \"Scanning for Live Hosts...\"");

  Keyboard.print("&& echo \"------------------------------\"");

  Keyboard.print("&& ping -c 3 15.151.298.31");

  Keyboard.print("&& ping -c 3 15.151.298.31");

  // Add more IP addresses to ping as needed
  Keyboard.print("&& echo \"------------------------------\"");

  Keyboard.print("&& echo \"Performing ARP Scan...\"");

  Keyboard.print("&& echo \"------------------------------\"");

  Keyboard.print("&& arp -a");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}