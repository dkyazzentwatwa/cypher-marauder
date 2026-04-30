/*
Title: iOS Shared Shortcut Demo
RuntimeSec: 18
Platform: ios_shared
RequiresFocusedTextField: false
*/

void setup() {}
void loop() {
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:700");
  typeCmd("TEXT:notes");
  typeCmd("WAIT:300");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:900");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:400");
  typeCmd("KEY:CMD+SPACE");
}
