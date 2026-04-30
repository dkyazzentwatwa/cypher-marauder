/*
Title: iPhone Focus Tools Demo
RuntimeSec: 28
Platform: iphone
RequiresFocusedTextField: false
*/

void setup() {}
void loop() {
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1100");
  typeCmd("TEXT:clock");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:weather");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:settings");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:maps");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
}
