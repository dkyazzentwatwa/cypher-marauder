#pragma once

#include <Arduino.h>

// This beginner fork ships only the Waveshare ESP32-S3 1.47 touch LCD build.
#define BOARD_HAS_DISPLAY 1
#define BOARD_HAS_TOUCH 1
#define BOARD_HAS_THREE_BUTTONS 0
#define BOARD_DISPLAY_IS_SSD1306 0

constexpr const char *BOARD_PROFILE_NAME = "waveshare-esp32s3-147-touch-lcd";
constexpr uint32_t SERIAL_BAUD = 115200;

constexpr uint16_t LCD_WIDTH = 172;
constexpr uint16_t LCD_HEIGHT = 320;
constexpr uint8_t LCD_ROTATION = 2;

constexpr int PIN_LCD_SCLK = 38;
constexpr int PIN_LCD_MOSI = 39;
constexpr int PIN_LCD_CS = 21;
constexpr int PIN_LCD_DC = 45;
constexpr int PIN_LCD_RST = 40;
constexpr int PIN_LCD_BL = 46;

constexpr int PIN_TOUCH_SDA = 42;
constexpr int PIN_TOUCH_SCL = 41;
constexpr int PIN_TOUCH_RST = 47;
constexpr int PIN_TOUCH_INT = 48;

constexpr int PIN_SD_CLK = 16;
constexpr int PIN_SD_CMD = 15;
constexpr int PIN_SD_D0 = 17;
constexpr int PIN_SD_D1 = 18;
constexpr int PIN_SD_D2 = 13;
constexpr int PIN_SD_D3 = 14;

constexpr int PIN_BOOT_BUTTON = 0;
constexpr bool BOOT_BUTTON_ACTIVE_LOW = true;
constexpr bool BOOT_BUTTON_AUTODETECT_POLARITY = false;

constexpr uint8_t DISPLAY_COL_OFFSET = 34;
constexpr uint8_t DISPLAY_ROW_OFFSET = 0;
constexpr uint8_t DISPLAY_SPI_MODE = SPI_MODE0;
constexpr bool DISPLAY_MIRROR_X = true;
constexpr bool DISPLAY_MIRROR_Y = false;

#ifndef ENABLE_TOUCH
#define ENABLE_TOUCH 1
#endif

#ifndef ENABLE_BLE_SCAN
#define ENABLE_BLE_SCAN 1
#endif

#ifndef ENABLE_HID_ACTIONS
#define ENABLE_HID_ACTIONS 1
#endif

constexpr bool HID_DEFAULT_ARMED = false;
constexpr const char *HID_ALLOWLIST_SOURCE = "payloads/";

constexpr const char *AP_SSID = "ESP32-S3-147";
constexpr const char *AP_PASSWORD = "waveshare147";
