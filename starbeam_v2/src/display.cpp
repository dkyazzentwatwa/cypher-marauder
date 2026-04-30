// display.cpp - Display Module Implementation for Project Starbeam V2
// Phase 1: Extract V1 logic (no optimizations yet)

#include "display.h"
#include "terminal.h"

// Static member initialization
Adafruit_SSD1306 Display::oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
U8G2_FOR_ADAFRUIT_GFX Display::u8g2;

// Menu labels array (must match MenuItem enum order in types.h)
const char* Display::menuLabels[NUM_MENU_ITEMS] = {
    "BT Jammer",
    "Drone Jammer",
    "Wifi Jammer",
    "CC1 Jammer",
    "CC1101 Scan",
    "NRF Scan",
    "WiFi Scanner",
    "WiFi Heatmap",
    "BLE Scanner",
    "Flock Detector",
    "Web Server ON",
    "Web Server OFF",
    "Web Status",
    "Deauth Target",
    "Deauth All",
    "Beacon Flood",
    "Probe Flood",
    "PMKID Capture",
    "NRF Test",
    "CC1101 Test",
    "Test HSPI",
    "CC Single",
    "CC2 SINGLE",
    "Rec Raw",
    "Play Raw",
    "Show Raw",
    "Show Buffer",
    "Get RSSI 433mhz",
    "Flush Buffer",
    "Stop CC1101s",
    "Reset CC1101",
    "Set 434.440mhz",
    "Set 434.30mhz",
    "Set 434.400mhz",
    "Set 433.90mhz",
    "Marauder Core",
    "Settings",
    "Help"
};

void Display::init() {
    if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Halt if display init fails
    }

    // Initialize U8g2 for Adafruit GFX
    u8g2.begin(oled);

    // Initial display sequence
    oled.display();
    delay(2000);
    oled.clearDisplay();
}

void Display::drawMenu(MenuItem selectedMenuItem, int firstVisibleMenuItem) {
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    u8g2.setFont(u8g2_font_baby_tf);

    // Title bar
    oled.fillRect(0, 0, SCREEN_WIDTH, 16, SSD1306_WHITE);
    oled.setTextColor(SSD1306_BLACK);
    oled.setCursor(5, 4);
    oled.setTextSize(1);
    oled.println("Home");

    // Menu items below title bar
    oled.setTextColor(SSD1306_WHITE);
    for (int i = 0; i < 2; i++) {  // Show 2 menu items at a time
        int menuIndex = (firstVisibleMenuItem + i) % NUM_MENU_ITEMS;
        int16_t x = 5;
        int16_t y = 20 + (i * 20);

        // Highlight the selected item
        if (selectedMenuItem == menuIndex) {
            oled.fillRect(0, y - 2, SCREEN_WIDTH, 15, SSD1306_WHITE);
            oled.setTextColor(SSD1306_BLACK);
        } else {
            oled.setTextColor(SSD1306_WHITE);
        }

        oled.setCursor(x, y);
        oled.setTextSize(1);
        oled.println(menuLabels[menuIndex]);
    }

    oled.display();

    // Echo menu to terminal
    String line1 = "MENU";
    String line2 = String(menuLabels[selectedMenuItem]);
    if (selectedMenuItem > 0) {
        line2 = "> " + line2;
    }
    String line3 = "[UP/DOWN/SELECT]";
    String line4 = "";
    Terminal::echoToSerial(line1, line2, line3, line4);
}

void Display::drawBorder() {
    oled.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
}

void Display::displayInfo(String title, String info1, String info2, String info3) {
    oled.clearDisplay();
    drawBorder();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);

    // Title
    oled.setCursor(4, 4);
    oled.println(title);
    oled.drawLine(0, 14, SCREEN_WIDTH, 14, SSD1306_WHITE);

    // Info lines
    oled.setCursor(4, 18);
    oled.println(info1);
    oled.setCursor(4, 28);
    oled.println(info2);
    oled.setCursor(4, 38);
    oled.println(info3);

    oled.display();

    // Echo to terminal
    Terminal::echoToSerial(title, info1, info2, info3);
}

void Display::updateDisplay(const char* message) {
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.print(message);
    oled.display();

    // Echo to terminal
    Terminal::echoToSerial(String(message), "", "", "");
}

void Display::displayTitleScreen() {
    oled.clearDisplay();
    u8g2.setFont(u8g2_font_adventurer_tr);
    u8g2.setCursor(20, 40);
    u8g2.print("CYPHER BOX");
    oled.display();
}

void Display::displayInfoScreen() {
    oled.clearDisplay();
    u8g2.setFont(u8g2_font_baby_tf);

    u8g2.setCursor(0, 22);
    u8g2.print("Welcome to PROJECT STARBEAM!");

    u8g2.setCursor(0, 30);
    u8g2.print("This is a cool cyber tool.");

    u8g2.setCursor(0, 38);
    u8g2.print("I perform analysis & attacks.");

    u8g2.setCursor(0, 46);
    u8g2.print("Add radios to have some fun!!");

    u8g2.setCursor(0, 54);
    u8g2.print("Have fun & be safe ~_~;");

    oled.display();
}

void Display::displayLegalWarning() {
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.println("LEGAL WARNING");
    oled.println("");
    oled.println("Security testing");
    oled.println("features are for");
    oled.println("AUTHORIZED use");
    oled.println("ONLY.");
    oled.println("");
    oled.println("[SELECT] Accept");
    oled.display();
}
