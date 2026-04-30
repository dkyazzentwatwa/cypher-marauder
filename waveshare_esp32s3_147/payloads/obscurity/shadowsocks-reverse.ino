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

  // Returns your wifi network back to normal.
  // Must use the Kill command with the PID number to end tor 'Kill PID#'
  // You can use the ps command in the terminal to check if the Shadowsocks
  // and Tor processes are running.
  // Command to check Shadowsocks: 'ps -ef | grep shadowsocks'
  // Command to check Tor: 'ps -ef | grep tor'
  // Author: Narsty
  // Title: Shadowsocks Tor Proxy Reverse
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

  delay(1000);

  Keyboard.print("networksetup -setsocksfirewallproxystate Wi-Fi off");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}