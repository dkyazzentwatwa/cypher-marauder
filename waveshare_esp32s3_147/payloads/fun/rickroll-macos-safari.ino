/*
Title: Rickroll Safari (macOS)
RuntimeSec: 35
Platform: macos
RequiresFocusedTextField: false
*/

void setup() {
  // Open Safari directly to the Rickroll URL and keep timing relaxed.
  typeCmd("open -a Safari 'https://www.youtube.com/watch?v=oHg5SJYRHA0'");
  typeCmd("sleep 10");
  typeCmd("osascript -e 'tell application \"Safari\" to activate'");
  typeCmd("sleep 6");
}

void loop() {}
