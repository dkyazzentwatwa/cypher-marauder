# Cypher Marauder

Cypher Marauder is a beginner-focused Arduino fork with only five supported board sketches. Open the folder for your board, open the matching `.ino`, and upload.

This repo now has two layers:

- The five board folders are the only supported upload targets.
- `shared/marauder_core/` is the portable Marauder feature layer fused into those targets.
- `shared/marauder_reference/` keeps the original Marauder source for future feature porting only; it is not a supported board target.

## Supported Boards

| Board | Open this sketch | Hardware profile | Compile command |
| --- | --- | --- | --- |
| Project Starbeam V2 | `starbeam_v2/starbeam_v2.ino` | ESP32 with Starbeam OLED/buttons, NRF24, and CC1101 modules | `arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=huge_app starbeam_v2` |
| ESP32 DevKitC | `esp32_devkitc/esp32_devkitc.ino` | Stock ESP32-DevKitC, serial only, no external modules | `arduino-cli compile --fqbn esp32:esp32:esp32 esp32_devkitc` |
| Waveshare ESP32-S3 1.47 | `waveshare_esp32s3_147/waveshare_esp32s3_147.ino` | Waveshare ESP32-S3 1.47 touch LCD | `arduino-cli compile --fqbn esp32:esp32:esp32s3:USBMode=hwcdc,PartitionScheme=huge_app waveshare_esp32s3_147` |
| Cypherbox | `cypherbox/cypherbox.ino` | Cypherbox OLED/buttons/SD/RFID/GPS build | `arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=huge_app cypherbox` |
| ESP32 devboard custom PCB | `esp32_devboard_custom/esp32_devboard_custom.ino` | Custom ESP32 devboard with SSD1306, 3 buttons, WS2812 | `arduino-cli compile --fqbn esp32:esp32:esp32 esp32_devboard_custom` |

## Arduino IDE Upload

1. Install the ESP32 board package in Arduino IDE.
2. Install the libraries listed below for your board.
3. Open the `.ino` file for your board.
4. Select the matching ESP32 board in **Tools > Board**.
5. Select the USB serial port in **Tools > Port**.
6. Click **Upload**.

## Arduino CLI Upload

Replace `/dev/cu.usbserial-0001` with your current port from `arduino-cli board list`.

```bash
arduino-cli compile --upload --fqbn esp32:esp32:esp32:PartitionScheme=huge_app --port /dev/cu.usbserial-0001 starbeam_v2
arduino-cli compile --upload --fqbn esp32:esp32:esp32 --port /dev/cu.usbserial-0001 esp32_devkitc
arduino-cli compile --upload --fqbn esp32:esp32:esp32s3:USBMode=hwcdc,PartitionScheme=huge_app --port /dev/cu.usbmodem1101 waveshare_esp32s3_147
arduino-cli compile --upload --fqbn esp32:esp32:esp32:PartitionScheme=huge_app --port /dev/cu.usbserial-0001 cypherbox
arduino-cli compile --upload --fqbn esp32:esp32:esp32 --port /dev/cu.usbserial-0001 esp32_devboard_custom
```

For the Waveshare ESP32-S3 board, do the 1200-baud touch first if the board does not enter upload mode cleanly:

```bash
stty -f /dev/cu.usbmodem1101 1200
sleep 2
arduino-cli board list
```

Then rerun the upload command with the new port shown by `arduino-cli board list`.

## Libraries

Install these with Arduino Library Manager or `arduino-cli lib install`.

| Board | Libraries |
| --- | --- |
| Starbeam V2 | `Adafruit GFX Library`, `Adafruit SSD1306`, `U8g2_for_Adafruit_GFX`, `RF24`, `SmartRC-CC1101-Driver-Lib`; the second CC1101 driver files are bundled in `starbeam_v2/src/` |
| ESP32 DevKitC | None beyond the ESP32 board package |
| Waveshare ESP32-S3 1.47 | `Adafruit GFX Library`, `Adafruit ST7735 and ST7789 Library`, `Adafruit SSD1306`, `NimBLE-Arduino` |
| Cypherbox | `Adafruit GFX Library`, `Adafruit SSD1306`, `U8g2_for_Adafruit_GFX`, `TinyGPSPlus`, `RTClib`, `MFRC522`, `Adafruit NeoPixel` |
| ESP32 devboard custom PCB | `Adafruit GFX Library`, `Adafruit SSD1306`, `FastLED` |

## Serial Monitor

All sketches use `115200` baud:

```bash
arduino-cli monitor --port /dev/cu.usbserial-0001 --baudrate 115200
```

## Fused Marauder Core

Every board exposes the shared Marauder core through serial commands. The custom ESP32 devboard also has an OLED **Marauder Core** menu item.

```text
marauder help
marauder status
marauder wifi
marauder list
marauder monitor
marauder monitor beacon
marauder monitor probe
marauder monitor deauth
marauder monitor eapol
marauder channel 6
marauder hop on
marauder hop off
marauder stop
marauder reset
```

Current shared-core features:

| Feature | What it does |
| --- | --- |
| AP discovery | Scans nearby access points, including hidden SSIDs, and prints channel/RSSI/security/BSSID/SSID. |
| Channel control | Sets a fixed monitor channel or hops channels 1-13. |
| Packet monitor | Uses ESP32 promiscuous mode for passive packet counting. |
| Marauder-style counters | Tracks management/data/control frames plus beacon, probe, deauth, disassoc, and EAPOL counts. |
| Board integration | Serial commands on all five boards; OLED menu/status screen on the custom ESP32 devboard. |

## Folder Rules

- Only the five folders in the support table are active board sketches.
- Each board sketch is self-contained inside this repo.
- Unsupported upstream Marauder variants, flash bundles, release binaries, PCB/mechanical archives, and old TFT setup headers were intentionally removed.
- Original Marauder source kept under `shared/marauder_reference/` is reference-only and should not be uploaded directly.
