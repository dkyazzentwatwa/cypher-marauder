#include "creator_input.h"

/*
Title: Reveal and Reset
Vibe: Behind-the-scenes ending
RuntimeSec: 60
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: This payload is itself the undo/reset sequence.
CameraNotes: Use as final clip segment to reveal scripted nature.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("pkill -f reel_ >/dev/null 2>&1 || true");
  typeCmd("killall Calculator Chess Music Preview Safari >/dev/null 2>&1 || true");
  typeCmd("clear");
  typeCmd("echo '========================================='");
  typeCmd("echo 'REVEAL: THIS WAS A CHOREOGRAPHED REEL'");
  typeCmd("echo 'No persistence, no credential access, no stealth.'");
  typeCmd("echo 'UNDO COMPLETE. YOU ARE BACK TO NORMAL.'");
  typeCmd("echo '========================================='");

  endKeyboard();
}

void loop() {}
