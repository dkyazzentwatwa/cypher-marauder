#include "creator_input.h"

/*
Title: Cinematic Fake Breach
Vibe: Chaotic hacker aesthetic
RuntimeSec: 75
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: pkill -f reel_ >/dev/null 2>&1 || true
CameraNotes: Shoot over-the-shoulder; start tight on terminal, then pull back on final reveal.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("clear");
  typeCmd("echo 'SCENE 1/3: ACCESS PHASE'");
  typeCmd(R"(bash -lc 'printf "\033[32m"; for i in $(seq 1 26); do printf "[%02d] injecting visual payload into relay_%02d ... OK\n" "$i" "$((RANDOM%97+1))"; sleep 0.35; done; printf "\n\033[1;92mSIMULATION COMPLETE - NO REAL ACCESS PERFORMED\033[0m\n"; sleep 10')");
  panicResetHint();

  endKeyboard();
}

void loop() {}
