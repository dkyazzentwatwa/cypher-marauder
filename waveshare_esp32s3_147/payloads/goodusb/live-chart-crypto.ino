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

  // This script fetches the current Bitcoin exchange rate using the "curl" command.
  // The data is retrieved from the rate.sx website.
  // You can Replace "Btc" with another popular asset of your choice
  // Title: Live Chart and price ticker
  // Author: Narsty
  // Version: 1.0
  // Category: GOODUSB
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("TERMINAL");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("curl rate.sx/btc");

  delay(500);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}