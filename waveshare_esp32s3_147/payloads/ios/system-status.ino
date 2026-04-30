/*
Title: System Status Template
RuntimeSec: 20
Platform: ios_simple
RequiresFocusedTextField: true
UseCase: Professional status reporting
*/

void setup() {}
void loop() {
  typeCmd("TEXT:--- DEVICE STATUS REPORT ---");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:Status: ONLINE");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:Mode: STEALTH");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:Battery: NOMINAL");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:Connectivity: SECURE");
  typeCmd("KEY:ENTER");
  typeCmd("TEXT:--------------------------");
  typeCmd("KEY:ENTER");
}
