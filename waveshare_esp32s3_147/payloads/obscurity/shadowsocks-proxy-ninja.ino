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

  // Shadowsocks encrypts your network traffic and routes it through the proxy server
  // making it difficult for third parties to analyze or decipher your data.
  // This can help protect your network traffic from being easily monitored or intercepted.
  // Requirements: Shadowsocks can be installed using command 'brew install shadowsocks-libev'
  // you can use the ps command in the terminal to check if the Shadowsocks process is running.
  // command 'ps -ef | grep shadowsocks'
  // Author: Narsty
  // Title: Shadowsocks Proxy Ninja
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

  Keyboard.print("brew services start shadowsocks-libev");

  typeKey(KEY_RETURN);

  delay(5000);

  Keyboard.print("networksetup -setsocksfirewallproxy Wi-Fi 127.0.0.1 1080");

  typeKey(KEY_RETURN);

  delay(250);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}