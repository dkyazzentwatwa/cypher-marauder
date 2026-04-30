/*
Title: Digit Storm
RuntimeSec: 15
Platform: ios_simple
RequiresFocusedTextField: true
UseCase: Stress test / Visual effect
*/

void setup() {}
void loop() {
  typeCmd("TEXT:1234567890");
  typeCmd("WAIT:100");
  typeCmd("TEXT:0987654321");
  typeCmd("WAIT:100");
  typeCmd("TEXT:13579");
  typeCmd("WAIT:100");
  typeCmd("TEXT:24680");
  typeCmd("WAIT:100");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:200");
}
