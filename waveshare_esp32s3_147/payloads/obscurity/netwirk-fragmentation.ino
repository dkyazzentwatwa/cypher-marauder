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

  // This script sets up a network fragmentation technique using Scapy.
  // It generates ICMP packets with a randomized payload length and performs fragmentation.
  // The payload is created by randomly selecting printable ASCII characters within a specific range.
  // The script constructs an IP packet with the specified destination IP address and ICMP payload.
  // It then fragments the packet into smaller fragments using a fragment size of 200 bytes.
  // The fragmented packets are sent with a delay of 0.1 seconds between each fragment.
  // The script continuously loops and sends the fragments to simulate fragmented network traffic.
  // Network fragmentation can introduce complexity
  // and potential challenges for packet inspection and reassembly.
  // This technique aims to add an additional layer of obfuscation
  // to the network traffic by breaking it into smaller fragments.
  // However, it does not guarantee complete anonymity or provide absolute security.
  // Adjust the range values in the payload generation
  // to control the length of the randomized payload.
  // The current range is set to generate payloads between 500 and 1500 characters.
  // Modify destination IP address in the 'IP(dst="10.0.0.1")' section to match your desired target.
  // Modify the fragment size (fragsize) and the delay between fragments (inter) as needed for your testing purposes.
  // Requirements: Python 3 and Scapy. You can install Scapy using the command 'pip3 install scapy'.
  // Author: Narsty
  // Title: Fragmentation Traffic Generator
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

  Keyboard.print("python3 -c 'import random; from scapy.all import *; payload = \"\".join(chr(random.randint(32, 126)) for _ in range(random.randint(500, 1500))); packet = IP(dst=\"10.0.0.1\") / ICMP() / payload; fragments = fragment(packet, fragsize=200); send(fragments, inter=0.1, loop=True)'");

  delay(500);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}