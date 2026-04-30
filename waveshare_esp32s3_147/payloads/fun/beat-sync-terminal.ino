#include "creator_input.h"

/*
Title: Beat Sync Terminal
Vibe: Rhythm-synced terminal pulses
RuntimeSec: 70
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: pkill -f reel_beat >/dev/null 2>&1 || true
CameraNotes: Align start to music drop; use 0.5x slow-mo for second half.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("clear");
  typeCmd("echo 'SCENE 1/3: 8-16-32 BAR VISUAL PULSE'");
  typeCmd(R"(bash -lc 'for bars in 8 16 32; do for i in $(seq 1 $bars); do printf "[BAR %02d/%02d] ########################\r" "$i" "$bars"; sleep 0.22; done; printf "\n"; done; echo "DROP COMPLETE"; sleep 12' # reel_beat)");
  panicResetHint();

  endKeyboard();
}

void loop() {}
