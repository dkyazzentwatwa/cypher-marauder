/*
Title: iPhone Home Loop Demo
RuntimeSec: 26
Platform: iphone
RequiresFocusedTextField: false
*/

void setup() {}
void loop() {
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1100");
  typeCmd("TEXT:safari");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:900");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:settings");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:900");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:photos");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
}
