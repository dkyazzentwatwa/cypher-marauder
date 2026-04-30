#include "creator_input.h"

/*
Title: Matrix Rain Studio
Vibe: Green terminal rain
RuntimeSec: 70
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: pkill -f reel_matrix >/dev/null 2>&1 || true
CameraNotes: Best as b-roll background; keep camera steady for readability.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("clear");
  typeCmd("echo 'SCENE 1/3: MATRIX RAIN'");
  typeCmd(R"(bash -lc 'printf "\033[0;32m"; for i in $(seq 1 220); do tr -dc "A-F0-9" </dev/urandom | head -c 64; printf "\n"; sleep 0.18; done; printf "\033[0m"; sleep 8' # reel_matrix)");
  panicResetHint();

  endKeyboard();
}

void loop() {}
