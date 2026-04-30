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

  // restore the network settings and visibility.
  // Reverses the commands in MacCloak: Untraceable
  // replace <original_hostname> with the original hostname of your system.
  // Author: Narsty
  // Title: MacCloak/Reverse
  // Target: MacOS
  // Version: 1.0
  // Category: Obscurity
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

  Keyboard.print("sudo dscacheutil -flushcache");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("defaults write com.apple.Safari CustomUserAgent \"\\\"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.93 Safari/537.36\\\"\"");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo defaults write /Library/Preferences/com.apple.locationd.plist LocationServicesEnabled -bool true");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("defaults write com.apple.Safari PrivateBrowsingEnabled -bool false");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("defaults write com.apple.Siri SuggestionsEnabled -bool true");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo scutil --set HostName <original_hostname>");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("open /Applications/Safari.app");

  typeKey(KEY_RETURN);

  delay(5000);

  Keyboard.print("history -c");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}