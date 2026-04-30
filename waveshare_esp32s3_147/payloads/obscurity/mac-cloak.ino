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

  // Clear DNS Cach
  // Randomize User Agent
  // Disable Location Services
  // Clear Browsing History
  // Enable Private Browsing Mode
  // Disable Siri Suggestions
  // Randomize Hostname
  // Clear Terminal History
  // Open Tor Browser
  // Please note that some commands may require administrative privileges (sudo).
  // Replace <random_hostname> with a random hostname of your choice.
  // Author: Narsty
  // Title: MacCloak: Untraceable
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

  delay(7000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("defaults write com.apple.Safari CustomUserAgent \"\\\"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3\\\"\"");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo defaults write /Library/Preferences/com.apple.locationd.plist LocationServicesEnabled -bool false");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("rm -rf ~/Library/Caches/com.apple.Safari/");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("defaults write com.apple.Safari PrivateBrowsingEnabled -bool true");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("defaults write com.apple.Siri SuggestionsEnabled -bool false");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo scutil --set HostName <random_hostname>");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("open /Applications/TorBrowser.app");

  typeKey(KEY_RETURN);

  delay(5000);

  Keyboard.print("history -c");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}