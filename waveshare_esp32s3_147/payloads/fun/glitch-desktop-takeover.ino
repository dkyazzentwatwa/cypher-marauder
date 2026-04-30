#include "creator_input.h"

/*
Title: Glitch Desktop Takeover
Vibe: Glitch chaos montage
RuntimeSec: 80
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: osascript -e 'tell application "System Events" to set dark mode of appearance preferences to false'; killall Terminal
CameraNotes: Capture full screen with quick cuts when apps switch.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("echo 'SCENE 1/3: VISUAL GLITCH WAVE'");
  typeCmd(R"(bash -lc 'for i in $(seq 1 12); do osascript -e "tell application \"System Events\" to set dark mode of appearance preferences to true" >/dev/null 2>&1; sleep 0.4; osascript -e "tell application \"System Events\" to set dark mode of appearance preferences to false" >/dev/null 2>&1; sleep 0.4; done')");
  typeCmd("open -a 'Calculator'; sleep 1; open -a 'Chess'; sleep 1; open -a 'Music'; sleep 1; open -a 'Preview'");
  typeCmd("echo 'SCENE 3/3: RESET PATH READY'");
  panicResetHint();

  endKeyboard();
}

void loop() {}
