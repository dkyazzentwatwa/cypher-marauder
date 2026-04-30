/*
Title: iOS Shared Rickroll Safari
RuntimeSec: 40
Platform: ios_shared
RequiresFocusedTextField: false
*/

void setup() {}

void loop() {
  // Open Spotlight, launch Safari, then type URL with longer waits for stability.
  typeCmd("KEY:CMD+SPACE");
  typeCmd("WAIT:1200");
  typeCmd("TEXT:safari");
  typeCmd("WAIT:900");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:2200");
  typeCmd("KEY:CMD+L");
  typeCmd("WAIT:900");
  typeCmd("TEXT:https://www.youtube.com/watch?v=oHg5SJYRHA0");
  typeCmd("WAIT:800");
  typeCmd("KEY:ENTER");
  typeCmd("WAIT:4500");
}
