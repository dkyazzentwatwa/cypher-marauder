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

  // Script that uses obfuscation to stealthily create a local HTTP server
  // hosting a simple web page on the machine.
  // access the server by opening a web browser and navigating to localhost:9090
  // Creates a new directory in the tmp folder named "ghost_server"
  // Creates a new HTML file named "index.html" with the message "Hello, this is the Silent Server!".
  // Starts a http.server server using Python's built-in SimpleHTTPServer module on port 9090.
  // The nohup command allows the server to continue running even after the terminal is closed
  // the output is redirected to /dev/null so that it doesn't appear in the terminal.
  // Author: Narsty
  // Title: Silent Server Creator
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

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("mkdir /tmp/ghost_server && cd /tmp/ghost_server");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo '<h1>Hello, this is the Phantom Server!</h1>' > index.html");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("nohup python3 -m http.server 9090 > /dev/null 2>&1 &");

  typeKey(KEY_RETURN);

  delay(500);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}