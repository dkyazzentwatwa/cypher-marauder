#include "creator_input.h"

/*
Title: App Chaos Montage
Vibe: Mac is possessed montage
RuntimeSec: 78
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: killall Calculator Chess Music Preview Safari Terminal >/dev/null 2>&1 || true
CameraNotes: Use quick jump cuts each time app focus changes.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("echo 'SCENE 1/3: APP CASCADE'");
  typeCmd(R"(bash -lc 'apps=("Safari" "Calculator" "Chess" "Preview" "Music"); for round in 1 2 3; do for app in "${apps[@]}"; do open -a "$app"; sleep 1.3; done; done; sleep 8' # reel_apps)");
  typeCmd("echo 'SCENE 3/3: MONTAGE ENDED'");
  panicResetHint();

  endKeyboard();
}

void loop() {}
