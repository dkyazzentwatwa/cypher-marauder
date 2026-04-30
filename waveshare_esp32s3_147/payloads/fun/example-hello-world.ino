/*
Title: Hello World
RuntimeSec: 5
*/

void setup() {
  // Each typeCmd() types the string into the terminal and presses Enter.
  // Commands run in order with automatic delays between them.
  // If a command contains `sleep N`, the firmware waits N seconds before
  // sending the next command — no manual timing needed.

  typeCmd("echo 'Hello from ESP32-S3 HID'");
  typeCmd("echo 'Add your commands here'");
}

void loop() {}
