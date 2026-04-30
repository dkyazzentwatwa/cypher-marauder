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

  // Web Scraping and Output to Desktop.
  // This script performs web scraping on a target website and saves the extracted data to a text file on the desktop.
  // Make sure to replace the target website URL and adjust the delay timings as needed for optimal performance.
  // Please note that web scraping may have legal and ethical implications,
  // script should only be performed on websites you have permission to access and scrape data from.
  // Replace 'https://www.example.com' with the target website URL.
  // Author: Narsty
  // Title: Web Scraper
  // Version: 1.0
  // Target: MacOs
  // Category: Recon
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(1000);

  Keyboard.print("cd ~/Desktop");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("touch output.txt");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("curl -s https://www.example.com > output.txt");

  delay(1000);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("open -e output.txt");

  delay(1000);

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}