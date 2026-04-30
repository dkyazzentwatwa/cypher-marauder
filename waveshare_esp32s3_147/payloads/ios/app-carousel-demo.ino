/*
Title: iPhone App Carousel Demo
RuntimeSec: 30
Platform: iphone
RequiresFocusedTextField: false
*/

void setup() {}
void loop() {
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1100");
  typeCmd("TEXT:notes");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1500");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:safari");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1500");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:photos");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1500");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:music");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1500");
  typeCmd("KEY:CMD+H");
}
