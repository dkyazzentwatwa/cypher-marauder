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

  // This only works against a Windows victim!
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(5000);

  Keyboard.print("textedit");

  typeKey(KEY_RETURN);

  delay(1000);

  typeKey(KEY_LEFT_ESC);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('n');
  Keyboard.releaseAll();

  Keyboard.print("------------------------");

  typeKey(KEY_RETURN);

  Keyboard.print("Hacked by Febi Mudiyanto");

  typeKey(KEY_RETURN);

  Keyboard.print("------------------------");

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}