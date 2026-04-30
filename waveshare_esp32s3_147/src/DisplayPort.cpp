#include "DisplayPort.h"

#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
#include <Adafruit_ST77xx.h>
#endif

namespace {
constexpr uint16_t COLOR_DARK_GREY = 0x7BEF;
constexpr uint16_t COLOR_LIGHT_GREY = 0xC618;
}

DisplayPort::DisplayPort()
#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
    : _tft(&SPI, PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RST), _nullGfx(LCD_WIDTH, LCD_HEIGHT) {}
#elif BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
    : _oled(LCD_WIDTH, LCD_HEIGHT, &Wire, PIN_SSD1306_RST), _nullGfx(LCD_WIDTH, LCD_HEIGHT) {}
#else
    : _nullGfx(128, 64) {}
#endif

bool DisplayPort::begin() {
#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
  pinMode(PIN_LCD_BL, OUTPUT);
  setBacklight(true);

  SPI.begin(PIN_LCD_SCLK, -1, PIN_LCD_MOSI, PIN_LCD_CS);
  _tft.init(LCD_WIDTH, LCD_HEIGHT, DISPLAY_SPI_MODE);
  _tft.setColRowStart(DISPLAY_COL_OFFSET, DISPLAY_ROW_OFFSET);
  _tft.setRotation(LCD_ROTATION);
  _tft.applyMadctlFix(LCD_ROTATION, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
  _tft.fillScreen(ST77XX_BLACK);
  _tft.setTextWrap(false);
  _tft.setTextSize(1);
  _tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  _ready = true;
    drawStatus("Cypher Marauder", "Display init OK");
  return true;
#elif BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
  Wire.begin(PIN_SSD1306_SDA, PIN_SSD1306_SCL);
  _ready = _oled.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDR);
  if (_ready) {
    _oled.clearDisplay();
    _oled.setTextWrap(false);
    _oled.setTextSize(1);
    _oled.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    _oled.setRotation(LCD_ROTATION);
    drawStatus("Cypher Marauder", "Display init OK");
  }
  return _ready;
#else
  _ready = false;
  return true;
#endif
}

Adafruit_GFX &DisplayPort::gfx() {
#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
  return _tft;
#elif BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
  return _oled;
#else
  return _nullGfx;
#endif
}

void DisplayPort::clear(uint16_t color) {
#if BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
  (void)color;
  _oled.clearDisplay();
#elif BOARD_HAS_DISPLAY
  _tft.fillScreen(color);
#else
  (void)color;
  return;
#endif
  flushIfNeeded();
}

void DisplayPort::drawHeader(const char *title) {
#if !BOARD_HAS_DISPLAY
  (void)title;
  return;
#else
  Adafruit_GFX &d = gfx();
#if BOARD_DISPLAY_IS_SSD1306
  d.fillRect(0, 0, width(), 10, SSD1306_BLACK);
  d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  d.setCursor(0, 1);
  d.print(title);
#else
  d.fillRect(0, 0, width(), 22, ST77XX_BLUE);
  d.setTextColor(ST77XX_WHITE, ST77XX_BLUE);
  d.setCursor(5, 7);
  d.print(title);
  d.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
#endif
  flushIfNeeded();
#endif
}

void DisplayPort::drawStatus(const String &line1, const String &line2) {
#if !BOARD_HAS_DISPLAY
  (void)line1;
  (void)line2;
  return;
#else
  clear();
  drawHeader("Status");
  Adafruit_GFX &d = gfx();
#if BOARD_DISPLAY_IS_SSD1306
  d.setCursor(0, 16);
  d.print(line1);
  if (line2.length() > 0) {
    d.setCursor(0, 28);
    d.print(line2);
  }
#else
  d.setCursor(8, 42);
  d.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  d.print(line1);
  if (line2.length() > 0) {
    d.setCursor(8, 58);
    d.print(line2);
  }
#endif
  flushIfNeeded();
#endif
}

void DisplayPort::drawMessage(const char *title, const String &body, const char *footer) {
#if !BOARD_HAS_DISPLAY
  (void)title;
  (void)body;
  (void)footer;
  return;
#else
  clear();
  drawHeader(title);
  Adafruit_GFX &d = gfx();
  int y = BOARD_DISPLAY_IS_SSD1306 ? 12 : 36;
  const int lineStep = BOARD_DISPLAY_IS_SSD1306 ? 10 : 14;
  int start = 0;
  while (start < body.length() && y < static_cast<int>(height() - (BOARD_DISPLAY_IS_SSD1306 ? 10 : 24))) {
    int end = body.indexOf('\n', start);
    if (end < 0) {
      end = body.length();
    }
    d.setCursor(BOARD_DISPLAY_IS_SSD1306 ? 0 : 6, y);
    d.print(body.substring(start, end));
    y += lineStep;
    start = end + 1;
  }
  drawFooter(footer);
#endif
}

void DisplayPort::drawFooter(const char *text) {
#if !BOARD_HAS_DISPLAY
  (void)text;
  return;
#else
  Adafruit_GFX &d = gfx();
#if BOARD_DISPLAY_IS_SSD1306
  d.fillRect(0, height() - 9, width(), 9, SSD1306_BLACK);
  d.drawFastHLine(0, height() - 10, width(), SSD1306_WHITE);
  d.setCursor(0, height() - 8);
  d.print(text);
#else
  d.fillRect(0, height() - 18, width(), 18, ST77XX_BLACK);
  d.drawFastHLine(0, height() - 19, width(), COLOR_DARK_GREY);
  d.setCursor(4, height() - 13);
  d.setTextColor(COLOR_LIGHT_GREY, ST77XX_BLACK);
  d.print(text);
#endif
  flushIfNeeded();
#endif
}

void DisplayPort::setBacklight(bool on) {
#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
  digitalWrite(PIN_LCD_BL, on ? HIGH : LOW);
#else
  (void)on;
#endif
}

void DisplayPort::setRotation(uint8_t rotation) {
#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
  _tft.setRotation(rotation);
  _tft.applyMadctlFix(rotation, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
#elif BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
  _oled.setRotation(rotation);
#else
  (void)rotation;
#endif
}

uint16_t DisplayPort::width() const {
#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
  return _tft.width();
#elif BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
  return _oled.width();
#else
  return _nullGfx.width();
#endif
}

uint16_t DisplayPort::height() const {
#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
  return _tft.height();
#elif BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
  return _oled.height();
#else
  return _nullGfx.height();
#endif
}

bool DisplayPort::available() const {
  return BOARD_HAS_DISPLAY && _ready;
}

void DisplayPort::flushIfNeeded() {
#if BOARD_HAS_DISPLAY && BOARD_DISPLAY_IS_SSD1306
  if (_ready) {
    _oled.display();
  }
#endif
}

#if BOARD_HAS_DISPLAY && !BOARD_DISPLAY_IS_SSD1306
void WaveshareST7789::applyMadctlFix(uint8_t rotation, bool mirrorX, bool mirrorY) {
  uint8_t madctl = 0;
  switch (rotation % 4) {
  case 0:
    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
    break;
  case 1:
    madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    break;
  case 2:
    madctl = ST77XX_MADCTL_RGB;
    break;
  case 3:
    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    break;
  }

  if (mirrorX) {
    madctl ^= ST77XX_MADCTL_MX;
  }
  if (mirrorY) {
    madctl ^= ST77XX_MADCTL_MY;
  }

  sendCommand(ST77XX_MADCTL, &madctl, 1);
}
#endif
