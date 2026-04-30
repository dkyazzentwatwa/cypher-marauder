#pragma once

#include <Arduino.h>
#include <SPI.h>
#if defined(ARDUINO_ARCH_ESP32) && !defined(ESP32)
#define ESP32 1
#endif
#include <Adafruit_GFX.h>

#include "../include/BoardConfig.h"

#ifndef ST77XX_BLACK
#define ST77XX_BLACK 0x0000
#define ST77XX_BLUE 0x001F
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_WHITE 0xFFFF
#endif

#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
#include <Adafruit_ST7789.h>
#include <SPI.h>
class WaveshareST7789 : public Adafruit_ST7789 {
public:
  WaveshareST7789(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst)
      : Adafruit_ST7789(spiClass, cs, dc, rst) {}

  using Adafruit_ST7789::setColRowStart;
  void applyMadctlFix(uint8_t rotation, bool mirrorX, bool mirrorY);
};
#elif BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#endif

class NullGfx : public Adafruit_GFX {
public:
  NullGfx(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {}
  void drawPixel(int16_t x, int16_t y, uint16_t color) override {
    (void)x;
    (void)y;
    (void)color;
  }
};

class DisplayPort {
public:
  DisplayPort();
  bool begin();
  Adafruit_GFX &gfx();

  void clear(uint16_t color = 0);
  void drawHeader(const char *title);
  void drawStatus(const String &line1, const String &line2 = String());
  void drawMessage(const char *title, const String &body, const char *footer);
  void drawFooter(const char *text);
  void setBacklight(bool on);
  void setRotation(uint8_t rotation);

  uint16_t width() const;
  uint16_t height() const;
  bool available() const;

private:
#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
  WaveshareST7789 _tft;
#elif BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
  Adafruit_SSD1306 _oled;
#endif
  NullGfx _nullGfx;
  bool _ready = false;

  void flushIfNeeded();
};
