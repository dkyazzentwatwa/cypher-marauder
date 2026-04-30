/*
Title: iPhone Productivity Tour Demo
RuntimeSec: 30
Platform: iphone
RequiresFocusedTextField: false
*/

void setup() {}
void loop() {
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1100");
  typeCmd("TEXT:reminders");
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
  typeCmd("TEXT:files");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
  typeCmd("WAIT:800");

  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1000");
  typeCmd("TEXT:shortcuts");
  typeCmd("WAIT:450");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:1400");
  typeCmd("KEY:CMD+H");
}
