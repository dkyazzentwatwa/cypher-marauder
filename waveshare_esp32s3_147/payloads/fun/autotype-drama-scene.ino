#include "creator_input.h"

/*
Title: Autotype Drama Scene
Vibe: Storytelling monologue
RuntimeSec: 85
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Undo: rm -f ~/Desktop/reel_drama_scene.txt
CameraNotes: Start on blank terminal, then cut to opened text file for reveal.
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  typeCmd("echo 'SCENE 1/3: MESSAGE BUILD'");
  typeCmd(R"(bash -lc 'printf "%s\n" "[LOG ENTRY]" "I thought the machine was haunted." "Then I realized it was choreography." "No breach. No malware. Just timing, typing, and camera angles." "When the beat drops, hit record." > ~/Desktop/reel_drama_scene.txt; open -a TextEdit ~/Desktop/reel_drama_scene.txt; sleep 12' # reel_drama)");
  panicResetHint();

  endKeyboard();
}

void loop() {}
