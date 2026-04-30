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

  // This script sets up a TCP randomization technique
  // Using Scapy to randomize the TCP packets by introducing random delays
  // between sending each packet.
  // This helps to obfuscate the packet timing.
  // Also adds an element of randomness to the network traffic.
  // Keep in mind that while it adds some level of randomness,
  // it doesn't guarantee complete anonymity or security.
  // This runs 1000 randomized TCP packets which last about 5 minutes.
  // You can change the 'send_packet(packet) for _ in range(1000)' parameter to extend the time.
  // For example if you change to 2000 this will last about 10 minutes.
  // Adjust the values of min_delay and max_delay as per your requirement.
  // Requirements:Python 3, and Scapy can install using the command 'pip3 install scapy'
  // warnings can be ignored as they indicate that no IPv4 address is currently assigned to those interfaces.
  // The script will continue to function as intended
  // Author: Narsty
  // Title: TCP Randomizer
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

  Keyboard.print("python3 -c 'import random, time; from scapy.all import *; min_delay = 0.1; max_delay = 0.5; packet = IP() / TCP(); send_packet = lambda pkt: (time.sleep(random.uniform(min_delay, max_delay)), send(pkt)); [send_packet(packet) for _ in range(1000)]'");

  delay(1000);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}