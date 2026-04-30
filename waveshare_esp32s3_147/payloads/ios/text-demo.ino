/*
Title: iOS Shared Text Demo
RuntimeSec: 20
Platform: ios_shared
RequiresFocusedTextField: true
*/

void setup() {}
void loop() {
  typeCmd("TEXT:ESP32-S3 iOS keyboard demo is active.");
  typeCmd("WAIT:400");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:This is benign sample text for notes, chat, or docs.");
  typeCmd("WAIT:400");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:Done.");
}
