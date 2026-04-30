/*
Title: iOS Notes Template Demo
RuntimeSec: 22
Platform: ios_shared
RequiresFocusedTextField: true
*/

void setup() {}
void loop() {
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:notes");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("TEXT:Meeting Notes");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:- Date:");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:- Participants:");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:- Action items:");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:  1)");
}
