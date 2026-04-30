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

  // The script will open the Terminal,
  // execute the nmap command to scan for open ports,
  // run nikto for web server vulnerability scanning.
  // Runs Zap to test for Network vulerabilities, prints report to Desktop. You can change location to your flipper on line 48.
  // Tshark command-line tool for capturing and analyzing network traffic. Will create a new file named capture.pcap on your desktop
  // You can change location to your flipper on line 51.
  // Finally the script performs directory and file brute-forcing with gobuster and password files
  // Must store password file on Desktop and name it "common.txt"
  // nmap, nikto, and gobuster can all be downloaded using homebrew/terminal
  // command 'brew install nmap && brew install nikto && brew install gobuster'
  // Download wireshark to use the terminal Tshark command
  // Download the OWASP ZAP.app file from the official OWASP ZAP website
  // nikto command will not execute if you don't have the correct port assigned!
  // verify that the web server is indeed running on an open port
  // as indicated by the Nmap scan, before using the code!
  // Line 45 should look like this after port # is inserted "STRING nikto -h 13.371.118.34 -p 73"
  // To use this script, replace <13.371.118.34 with target_ip>
  // Author: Narsty
  // Title: DarkStorm bruteforce
  // Target: MacOS
  // Version: 1.0
  // Category: Execution
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

  Keyboard.print("nmap -p 1-1000 -T4 -Pn 13.371.118.34");

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.print("nikto -h 13.371.118.34 -p <Open port # goes here>");

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.print("/Applications/OWASP\\ ZAP.app/Contents/Java/zap.sh -cmd -quickurl http://13.371.118.34 -quickout ~/Desktop/quick_scan_results.html");

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.print("tshark -i en0 -w ~/Desktop/capture.pcap");

  typeKey(KEY_RETURN);

  delay(7000);

  Keyboard.print("gobuster dir -u http://13.371.118.34 -w ~/Desktop/common.txt -t 50 -q");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}