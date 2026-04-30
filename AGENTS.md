# Repository Guidelines

## Project Shape

Cypher Marauder is Arduino-first ESP32 firmware with exactly five supported upload targets:

- `starbeam_v2/starbeam_v2.ino`
- `esp32_devkitc/esp32_devkitc.ino`
- `waveshare_esp32s3_147/waveshare_esp32s3_147.ino`
- `cypherbox/cypherbox.ino`
- `esp32_devboard_custom/esp32_devboard_custom.ino`

Do not add upstream Marauder board variants back as supported sketches. `shared/marauder_reference/` is reference-only integration material. Active portable behavior belongs in `shared/marauder_core/`, then must be copied into each sketch-local `src/MarauderCore.h` so Arduino IDE users can open a single folder and compile.

## Build Commands

Use Arduino CLI from the repo root:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=huge_app starbeam_v2
arduino-cli compile --fqbn esp32:esp32:esp32 esp32_devkitc
arduino-cli compile --fqbn esp32:esp32:esp32s3:USBMode=hwcdc,FlashSize=16M,PartitionScheme=huge_app waveshare_esp32s3_147
arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=huge_app cypherbox
arduino-cli compile --fqbn esp32:esp32:esp32 esp32_devboard_custom
```

For Waveshare upload issues, use the 1200-baud touch first, then re-check `arduino-cli board list`.

## Coding Rules

- Keep each board folder self-contained and beginner-friendly.
- Preserve existing board-specific modules for Starbeam radios, Cypherbox RFID/GPS/SD, Waveshare touch/HID/SD, and custom devboard OLED/buttons.
- Prefer shared command/menu behavior in `MarauderCore`; keep hardware pin maps and device drivers board-local.
- If `shared/marauder_core/MarauderCore.h` changes, sync it to all five `src/MarauderCore.h` copies and verify they match.
- Do not introduce absolute paths to neighboring source repos.
- Avoid unrelated formatting churn and do not restore deleted flash bundles, PCB/mechanical archives, release binaries, or old `User_Setup*` files.

## Safety Boundary

Active WiFi and portal lab features must stay gated behind the current boot’s lab unlock flow. Keep warnings clear and do not add new offensive capability beyond the existing Marauder/imported project surface. Default behavior should be passive scan/monitor/logging unless the user explicitly enters lab mode.

## Validation

Before handing off firmware changes:

- Compile all five sketches with the commands above.
- Confirm every sketch folder name still matches its `.ino`.
- Run stale-reference checks for unsupported upstream boards and absolute imported-repo paths.
- Note any hardware behavior that was not physically flashed or tested.
