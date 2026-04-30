/*
Title: iPhone Shortcut Demo
RuntimeSec: 20
Platform: iphone
RequiresFocusedTextField: false
*/

void setup() {}
void loop() {
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1100");
  typeCmd("TEXT:safari");
  typeCmd("WAIT:500");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1300");
  typeCmd("KEY:CMD+L");
  typeCmd("WAIT:450");
  typeCmd("TEXT:apple.com");
  typeCmd("KEY:ENTER");
}
