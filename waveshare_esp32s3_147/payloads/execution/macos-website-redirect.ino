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

  // Description: Opens Terminal and redirects to URL of choice
  // You must enter the desired website in the Url String DO NOT remove single Quotes!
  // Title: Website Redirect
  // Author: NARSTY
  // Target: MacOS
  // Version: 1.0
  // Category: Execution
  // ID 05ac:021e Apple:Keyboard
  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(200);

  Keyboard.print("terminal");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("open -a Safari 'Enter URL/Website here'");

  delay(1000);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}