/*
  ESP32 Dev Board Base Sketch

  A turnkey firmware for the ESP32 development board featuring:
  - SSD1306 0.96" OLED display (I2C)
  - 3 tactile buttons for navigation
  - WS2812 RGB LED for visual feedback
  - Interactive GPIO testing interface

  Features:
  - Menu-driven UI for testing GPIO pins (on/off)
  - LED color test with visual feedback
  - Board info display
  - Non-blocking button edge detection
  - Serial debug output (115200 baud)

  Required Libraries:
  - Adafruit_GFX
  - Adafruit_SSD1306
  - FastLED

  Install via Arduino IDE: Sketch -> Include Library -> Manage Libraries
*/

// ============================================================================
// INCLUDES
// ============================================================================

#include <Wire.h>
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MarauderCore.h"

// ============================================================================
// PIN DEFINITIONS
// ============================================================================

#define OLED_SDA        5
#define OLED_SCL        4
#define BTN_LEFT        34    // Input-only pin, active LOW, external pullup
#define BTN_CENTER      36    // Input-only pin, active LOW, external pullup
#define BTN_RIGHT       39    // Input-only pin, active LOW, external pullup
#define LED_PIN         27
#define NUM_LEDS        1

// ============================================================================
// DISPLAY SETUP
// ============================================================================

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool displayFound = false;

// ============================================================================
// LED SETUP
// ============================================================================

CRGB leds[NUM_LEDS];

// ============================================================================
// TESTABLE GPIO PINS
// ============================================================================

// Safe output-capable pins exposed on headers
// Excludes: UART(1,3), I2C(21,22), WS2812(27), flash(6-11), input-only(34,35,36,39)
// H1: IO23, IO19, IO18, IO25, IO26  (skip IO5=SDA, IO1/IO3=UART)
// H2: IO13, IO12, IO14, IO15, IO21, IO22, IO17, IO16  (IO21/22 free — PCB I2C is on IO4/5)
// H3: IO33, IO32, IO2, IO0  (skip IO4=SCL, IO35=input-only)
// Excludes: UART(1,3), I2C(4,5), WS2812(27), flash(6-11), input-only(34,35,36,39)
const uint8_t TEST_PINS[] = {
  0, 2, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 32, 33
};
const uint8_t NUM_TEST_PINS = sizeof(TEST_PINS) / sizeof(TEST_PINS[0]);

bool pinStates[17] = {false};

// ============================================================================
// MENU STATE
// ============================================================================

enum Screen {
  SCREEN_MAIN_MENU = 0,
  SCREEN_GPIO_TEST = 1,
  SCREEN_LED_TEST  = 2,
  SCREEN_BOARD_INFO = 3,
  SCREEN_MARAUDER_CORE = 4
};

enum LedColor {
  LED_OFF     = 0,
  LED_RED     = 1,
  LED_GREEN   = 2,
  LED_BLUE    = 3,
  LED_WHITE   = 4,
  LED_RAINBOW = 5
};

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void updateButtons();
bool wasPressed(uint8_t idx);
void handleButtonPresses();
void updateLED();
void renderScreen();
void renderMainMenu();
void renderGPIOTest();
void renderLEDTest();
void renderBoardInfo();
void renderMarauderCore();
void displayInit();
void displayBootScreen();
void processSerialCommands();
const char* getHeaderName(uint8_t pin);
const char* getColorName(LedColor color);

// ============================================================================
// GLOBAL STATE
// ============================================================================

Screen currentScreen = SCREEN_MAIN_MENU;
uint8_t menuIndex    = 0;
uint8_t gpioIndex    = 0;
LedColor currentLedColor = LED_OFF;
bool ledEnabled      = false;
uint8_t rainbowHue   = 0;
bool screenDirty     = true;
String serialLine;
unsigned long lastMarauderRefresh = 0;

// ============================================================================
// BUTTON STATE — edge detection
// ============================================================================

struct Button {
  uint8_t  pin;
  bool     lastRaw;
  bool     stable;
  unsigned long lastChangeTime;
  unsigned long pressStartTime;  // when button was first held
  bool     pressEvent;
  bool     holdEvent;
};

Button buttons[3] = {
  {BTN_LEFT,   true, true, 0, 0, false, false},
  {BTN_CENTER, true, true, 0, 0, false, false},
  {BTN_RIGHT,  true, true, 0, 0, false, false}
};

const unsigned long DEBOUNCE_MS = 50;
const unsigned long HOLD_MS     = 800;

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  // I2C first for display stability
  Wire.begin(OLED_SDA, OLED_SCL);

  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Cypher Marauder Booting ===");
  Serial.println("Target: ESP32 devboard custom");

  displayInit();

  // Buttons — input-only pins, no internal pullups available
  pinMode(BTN_LEFT,   INPUT);
  pinMode(BTN_CENTER, INPUT);
  pinMode(BTN_RIGHT,  INPUT);

  for (uint8_t i = 0; i < 3; i++) {
    buttons[i].lastRaw = digitalRead(buttons[i].pin);
    buttons[i].stable  = buttons[i].lastRaw;
  }
  Serial.println("OK: Buttons initialized");

  // Initialize GPIO test pins as OUTPUT LOW
  for (uint8_t i = 0; i < NUM_TEST_PINS; i++) {
    pinMode(TEST_PINS[i], OUTPUT);
    digitalWrite(TEST_PINS[i], LOW);
  }
  Serial.println("OK: GPIO pins initialized (all LOW)");

  // Initialize WS2812
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  leds[0] = CRGB::Black;
  FastLED.show();
  Serial.println("OK: WS2812 initialized");
  MarauderCore::core().begin("esp32-devboard-custom");

  Serial.println("=== Cypher Marauder Ready ===\n");
  screenDirty = true;
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  updateButtons();
  handleButtonPresses();
  updateLED();
  processSerialCommands();
  MarauderCore::core().poll();
  if (currentScreen == SCREEN_MARAUDER_CORE && millis() - lastMarauderRefresh > 500) {
    lastMarauderRefresh = millis();
    screenDirty = true;
  }

  if (screenDirty && displayFound) {
    renderScreen();
    screenDirty = false;
  }

  delay(10);
}

// ============================================================================
// BUTTON HANDLING — edge detection
// ============================================================================

void updateButtons() {
  unsigned long now = millis();

  for (uint8_t i = 0; i < 3; i++) {
    bool raw = digitalRead(buttons[i].pin);

    buttons[i].pressEvent = false;
    buttons[i].holdEvent  = false;

    if (raw != buttons[i].lastRaw) {
      buttons[i].lastChangeTime = now;
      buttons[i].lastRaw = raw;
    }

    if ((now - buttons[i].lastChangeTime) > DEBOUNCE_MS) {
      if (raw == LOW && buttons[i].stable == HIGH) {
        // Falling edge — button just pressed
        buttons[i].pressEvent    = true;
        buttons[i].pressStartTime = now;
      } else if (raw == LOW && buttons[i].stable == LOW) {
        // Still held — fire hold event once threshold is crossed
        if (!buttons[i].holdEvent &&
            (now - buttons[i].pressStartTime) >= HOLD_MS) {
          buttons[i].holdEvent     = true;
          buttons[i].pressStartTime = now; // reset so it doesn't re-fire
        }
      }
      buttons[i].stable = raw;
    }
  }
}

bool wasPressed(uint8_t idx) { return buttons[idx].pressEvent; }
bool wasHeld(uint8_t idx)    { return buttons[idx].holdEvent;  }

void handleButtonPresses() {
  bool left   = wasPressed(0);
  bool center = wasPressed(1);
  bool right  = wasPressed(2);

  bool anyHold = wasHeld(0) || wasHeld(1) || wasHeld(2);
  if (!left && !center && !right && !anyHold) return;

  if (currentScreen == SCREEN_MAIN_MENU) {
    if (left) {
      menuIndex = (menuIndex + 3) % 4;
      screenDirty = true;
      Serial.printf("MENU: left -> item %d\n", menuIndex);
    }
    if (right) {
      menuIndex = (menuIndex + 1) % 4;
      screenDirty = true;
      Serial.printf("MENU: right -> item %d\n", menuIndex);
    }
    if (center) {
      currentScreen = (Screen)(menuIndex + 1);
      screenDirty = true;
      Serial.printf("MENU: select -> screen %d\n", currentScreen);
    }
  }
  else if (currentScreen == SCREEN_GPIO_TEST) {
    // Hold LEFT or RIGHT to exit back to menu
    if (wasHeld(0) || wasHeld(2)) {
      currentScreen = SCREEN_MAIN_MENU;
      screenDirty = true;
      Serial.println("GPIO: hold -> main menu");
      return;
    }
    if (left) {
      gpioIndex = (gpioIndex + NUM_TEST_PINS - 1) % NUM_TEST_PINS;
      screenDirty = true;
      Serial.printf("GPIO: selected IO%d\n", TEST_PINS[gpioIndex]);
    }
    if (right) {
      gpioIndex = (gpioIndex + 1) % NUM_TEST_PINS;
      screenDirty = true;
      Serial.printf("GPIO: selected IO%d\n", TEST_PINS[gpioIndex]);
    }
    if (center) {
      pinStates[gpioIndex] = !pinStates[gpioIndex];
      digitalWrite(TEST_PINS[gpioIndex], pinStates[gpioIndex] ? HIGH : LOW);
      screenDirty = true;
      Serial.printf("GPIO: IO%d -> %s\n", TEST_PINS[gpioIndex],
                    pinStates[gpioIndex] ? "HIGH" : "LOW");
    }
  }
  else if (currentScreen == SCREEN_LED_TEST) {
    if (left) {
      currentLedColor = (LedColor)((currentLedColor + 5) % 6);
      ledEnabled = (currentLedColor != LED_OFF);
      screenDirty = true;
      Serial.printf("LED: color -> %s\n", getColorName(currentLedColor));
    }
    if (right) {
      currentLedColor = (LedColor)((currentLedColor + 1) % 6);
      ledEnabled = (currentLedColor != LED_OFF);
      screenDirty = true;
      Serial.printf("LED: color -> %s\n", getColorName(currentLedColor));
    }
    if (center) {
      currentScreen = SCREEN_MAIN_MENU;
      ledEnabled = false;
      leds[0] = CRGB::Black;
      FastLED.show();
      screenDirty = true;
      Serial.println("LED: exit -> main menu");
    }
  }
  else if (currentScreen == SCREEN_BOARD_INFO) {
    if (center || left || right) {
      currentScreen = SCREEN_MAIN_MENU;
      screenDirty = true;
      Serial.println("INFO: exit -> main menu");
    }
  }
  else if (currentScreen == SCREEN_MARAUDER_CORE) {
    if (left) {
      MarauderCore::core().menuPrev();
      screenDirty = true;
    }
    if (right) {
      MarauderCore::core().menuNext();
      screenDirty = true;
    }
    if (center) {
      MarauderCore::core().menuSelect();
      screenDirty = true;
    }
    if (wasHeld(0) || wasHeld(2)) {
      MarauderCore::core().stopMonitor();
      currentScreen = SCREEN_MAIN_MENU;
      screenDirty = true;
    }
  }
}

// ============================================================================
// LED UPDATE
// ============================================================================

void updateLED() {
  if (!ledEnabled) return;

  switch (currentLedColor) {
    case LED_RED:     leds[0] = CRGB::Red;                         break;
    case LED_GREEN:   leds[0] = CRGB::Green;                       break;
    case LED_BLUE:    leds[0] = CRGB::Blue;                        break;
    case LED_WHITE:   leds[0] = CRGB::White;                       break;
    case LED_RAINBOW: leds[0] = CHSV(rainbowHue, 255, 255);
                      rainbowHue = (rainbowHue + 2) % 256;         break;
    case LED_OFF:
    default:          leds[0] = CRGB::Black;                       break;
  }

  FastLED.show();
}

// ============================================================================
// DISPLAY RENDERING
// ============================================================================

void displayInit() {
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    displayFound = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Cypher Marauder");
    display.println("Starting...");
    display.display();
    Serial.println("OK: Display initialized");
  } else {
    displayFound = false;
    Serial.println("WARN: Display not found, continuing without it");
  }
}

void displayBootScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("CYPHER");
  display.println("MARAUDER");
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.println("Initializing...");
  display.display();
  delay(1500);
}

void renderScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  switch (currentScreen) {
    case SCREEN_MAIN_MENU:  renderMainMenu();  break;
    case SCREEN_GPIO_TEST:  renderGPIOTest();  break;
    case SCREEN_LED_TEST:   renderLEDTest();   break;
    case SCREEN_BOARD_INFO: renderBoardInfo(); break;
    case SCREEN_MARAUDER_CORE: renderMarauderCore(); break;
  }

  display.display();
}

void renderMainMenu() {
  display.setCursor(0, 0);
  display.println("== MAIN MENU ==");
  display.println();

  const char* items[] = {"GPIO Test", "LED Test", "Board Info", "Marauder Core"};
  for (uint8_t i = 0; i < 4; i++) {
    display.print(i == menuIndex ? "> " : "  ");
    display.println(items[i]);
  }

  display.println();
  display.println("L/R:nav  C:select");
}

void renderGPIOTest() {
  uint8_t pin   = TEST_PINS[gpioIndex];
  bool    state = pinStates[gpioIndex];

  display.setCursor(0, 0);
  display.println("== GPIO TEST ==");

  display.setTextSize(2);
  display.setCursor(0, 18);
  display.printf("IO%-2d", pin);

  display.setCursor(70, 18);
  display.print(state ? "HIGH" : "LOW ");

  display.setTextSize(1);
  display.setCursor(0, 42);
  display.printf("Header: %s  [%d/%d]", getHeaderName(pin), gpioIndex + 1, NUM_TEST_PINS);

  display.setCursor(0, 54);
  display.println("L/R:pin C:tog HLD:back");
}

void renderLEDTest() {
  display.setCursor(0, 0);
  display.println("== LED TEST ==");
  display.println();

  display.setTextSize(2);
  display.println(getColorName(currentLedColor));
  display.setTextSize(1);
  display.println();
  display.println("L/R:color  C:back");
}

void renderBoardInfo() {
  display.setCursor(0, 0);
  display.println("== BOARD INFO ==");
  display.println("ESP32-DEVKITC");
  display.println("SSD1306 @ IO21/22");
  display.println("Btns: IO34/36/39");
  display.println("WS2812 @ IO27");
  display.println("17 testable GPIOs");
  display.println("Any btn: back");
}

void renderMarauderCore() {
  String line1, line2, line3, line4;
  MarauderCore::core().renderMenuLines(line1, line2, line3, line4);
  display.setCursor(0, 0);
  display.println(line1);
  display.println(line2);
  display.println(line3);
  display.println(line4);
  display.println("C:select L/R:nav");
  display.println("Hold L/R:back");
}

// ============================================================================
// UTILITY
// ============================================================================

const char* getHeaderName(uint8_t pin) {
  // H1: IO23, IO19, IO18, IO5(I2C), IO25, IO26, IO1(UART), IO3(UART)
  if (pin == 23 || pin == 19 || pin == 18 ||
      pin == 25 || pin == 26)                            return "H1";
  // H2: IO13, IO12, IO14, IO15, IO21, IO22, IO17, IO16
  if (pin == 13 || pin == 12 || pin == 14 || pin == 15 ||
      pin == 21 || pin == 22 || pin == 17 || pin == 16)  return "H2";
  // H3: IO4(I2C), IO33, IO32, IO35(in-only), IO2, IO0
  if (pin == 33 || pin == 32 || pin == 2  || pin == 0)  return "H3";
  return "??";
}

const char* getColorName(LedColor color) {
  switch (color) {
    case LED_OFF:     return "OFF";
    case LED_RED:     return "RED";
    case LED_GREEN:   return "GREEN";
    case LED_BLUE:    return "BLUE";
    case LED_WHITE:   return "WHITE";
    case LED_RAINBOW: return "RAINBOW";
    default:          return "???";
  }
}

void processSerialCommands() {
  while (Serial.available() > 0) {
    char c = static_cast<char>(Serial.read());
    if (c == '\r') {
      continue;
    }
    if (c == '\n') {
      serialLine.trim();
      if (serialLine.length() > 0 && !MarauderCore::core().handleCommand(serialLine)) {
        Serial.println("Unknown command. Try 'marauder help'.");
      }
      serialLine = "";
    } else if (isPrintable(c)) {
      serialLine += c;
    }
  }
}
