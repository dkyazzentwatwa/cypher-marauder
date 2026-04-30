/*
Title: iPhone Card Stack Demo
RuntimeSec: 28
Platform: iphone
RequiresFocusedTextField: false
*/

void setup() {}
void loop() {
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1100");
  typeCmd("TEXT:photos");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:music");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:calendar");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:notes");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
}
