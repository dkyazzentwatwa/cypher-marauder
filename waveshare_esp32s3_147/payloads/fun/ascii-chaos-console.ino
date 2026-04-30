#include "creator_input.h"

/*
Title: ASCII Chaos Console
Vibe: Cafe hacker theater
RuntimeSec: 88
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: pkill -f reel_ascii >/dev/null 2>&1 || true
CameraNotes: Keep frame tight on terminal; start recording 2s before launch for dramatic build.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("clear");
  typeCmd("echo 'SCENE 1/3: BOOTSTRAP ASCII GRID'");
  typeCmd("echo 'module[vision] module[relay] module[crypto] module[signal] -> preflight' ");
  typeCmd(R"(bash -lc 'printf "\033[1;36m"; echo ":: uplink handshake accepted"; echo ":: rendering glyph stream"; printf "\033[0;32m"; chars="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()[]{}<>?/|\\-_=+~"; for i in $(seq 1 200); do line=""; for j in $(seq 1 84); do idx=$((RANDOM % ${#chars})); line+="${chars:$idx:1}"; done; printf "%s\n" "$line"; if [ $((i % 20)) -eq 0 ]; then printf "\033[1;33m[STATUS] relay_%02d synced | cache=%02d%% | entropy=OK\033[0m\n" "$((RANDOM%99+1))" "$((RANDOM%100))"; fi; sleep 0.08; done; printf "\033[1;92mSIMULATION COMPLETE - VISUALS ONLY\033[0m\n"; sleep 8' # reel_ascii)");
  typeCmd("echo 'SCENE 3/3: DROP COMPLETE'");
  panicResetHint();

  endKeyboard();
}

void loop() {}
