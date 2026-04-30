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

  // Ducky Script to Restore Disabled Network Interfaces
  // Author: Narsty
  // Title: MacNetGhostRestore
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

  Keyboard.print("sudo ifconfig en0 up");

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.print("sudo ifconfig en1 up");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en2 up");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en3 up");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en4 up");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en5 up");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en6 up");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en7 up");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en8 up");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo ifconfig en9 up");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("exit");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}