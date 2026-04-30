// display.h - Display Module for Project Starbeam V2
// Phase 1: Extract V1 logic into module (no optimizations yet)

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "../config.h"
#include "../types.h"

class Display {
public:
    // Initialization
    static void init();

    // Main menu display
    static void drawMenu(MenuItem selectedMenuItem, int firstVisibleMenuItem);

    // Info display (used by various operations)
    static void displayInfo(String title, String info1 = "", String info2 = "", String info3 = "");

    // Simple message display
    static void updateDisplay(const char* message);

    // Splash screens
    static void displayTitleScreen();
    static void displayInfoScreen();

    // Legal warning for security testing features
    static void displayLegalWarning();

    // Utility
    static void drawBorder();

    // Access to display objects (for operations that need direct access)
    static Adafruit_SSD1306& getOled() { return oled; }
    static U8G2_FOR_ADAFRUIT_GFX& getU8g2() { return u8g2; }

private:
    static Adafruit_SSD1306 oled;
    static U8G2_FOR_ADAFRUIT_GFX u8g2;

    // Menu labels array
    static const char* menuLabels[NUM_MENU_ITEMS];
};

#endif // DISPLAY_H
