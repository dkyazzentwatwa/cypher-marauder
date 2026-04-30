#include "creator_input.h"

/*
Title: Notification Storm Lite
Vibe: Meme panic then control
RuntimeSec: 65
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: defaults delete com.apple.notificationcenterui doNotDisturb >/dev/null 2>&1 || true
CameraNotes: Film reactions; frame top-right area where notifications appear.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("echo 'SCENE 1/3: NOTIFICATION BURST'");
  typeCmd(R"(bash -lc 'for i in $(seq 1 18); do osascript -e "display notification \"Signal spike $i/18\" with title \"REEL MODE\" subtitle \"Visual simulation\""; sleep 0.35; done; sleep 10' # reel_notify)");
  typeCmd("echo 'SCENE 3/3: BURST FINISHED'");
  panicResetHint();

  endKeyboard();
}

void loop() {}
