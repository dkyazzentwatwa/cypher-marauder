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

  // Title: BLUETOOTH ON
  // Author: NARSTY
  // Description: Opens spotlight, searches for bluetooth file exchange
  // and turns bluetooth on
  // using the Macs built in bluetooth File Exchange
  // Target: MacOS
  // Version: 1.0
  // Category: Execution
  // ID 05ac:021e Apple:Keyboard
  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(1000);

  Keyboard.print("bluetooth File Exchange");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(1000);

  typeKey(KEY_RETURN);

  delay(2000);

  typeKey(KEY_TAB);

  delay(2000);

  typeKey(' ');

  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}