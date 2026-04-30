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

  // This script sets up a network padding technique.
  // Using Scapy to send padded IP packets with random payload length.
  // Each packet is padded with a random number of 'A' characters to obfuscate the payload.
  // This helps to add padding to the network traffic and increase the overall packet size.
  // Keep in mind that while network padding can add some level of privacy,
  // it doesn't guarantee complete anonymity or security.
  // By default, it sends 600 packets with random payload length and padding.
  // Current script runs for about 5 minutes
  // Change the range value in the 'send([packet] * 600)' line 33
  // For example, changing it to 'send([packet] * 1200)' will last about 10 minutes.
  // Adjust the inter parameter to control the delay between each packet.
  // The current value is set to 0.5 seconds, but you can modify it as needed for your testing purposes.
  // Requirements: Python 3 and Scapy. You can install Scapy using the command 'pip3 install scapy'.
  // warnings can be ignored as they indicate that no IPv4 address is currently assigned to those interfaces.
  // The script will continue to function as intended
  // Author: Narsty
  // Title: Network Padding Fury
  // Target: MacOS
  // Version: 1.0
  // Category: Obscurity
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("Terminal");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("python3 -c 'import random, time; from scapy.all import *; packet = IP() / Padding(load=\"A\" * random.randint(100, 200)); send([packet] * 600, inter=0.5)'");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}