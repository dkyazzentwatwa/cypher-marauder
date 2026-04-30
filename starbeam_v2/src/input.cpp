// input.cpp - Input Handling Module Implementation for Project Starbeam V2
// Phase 1: Extract V1 logic (with blocking delays - will optimize in Phase 2)

#include "input.h"
#include "display.h"

// Static member initialization
bool Input::buttonPressed = false;

void Input::init() {
    // Configure button pins
    pinMode(BUTTON_UP, INPUT);
    pinMode(BUTTON_DOWN, INPUT);
    pinMode(BUTTON_SELECT, INPUT);

    // Configure LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

bool Input::isButtonPressed(uint8_t pin) {
    if (digitalRead(pin) == LOW) {
        delay(100);  // Debounce delay (V1 style - blocking)
        if (digitalRead(pin) == LOW) {
            digitalWrite(LED_PIN, HIGH);  // Turn on LED
            return true;
        }
    }
    return false;
}

void Input::handleMenuSelection(MenuItem& selectedMenuItem, int& firstVisibleMenuItem) {
    if (!buttonPressed) {
        if (isButtonPressed(BUTTON_UP)) {
            // Wrap around if at the top
            selectedMenuItem = static_cast<MenuItem>((selectedMenuItem == 0) ? (NUM_MENU_ITEMS - 1) : (selectedMenuItem - 1));

            if (selectedMenuItem == (NUM_MENU_ITEMS - 1)) {
                // If wrapped to the bottom, make it visible
                firstVisibleMenuItem = NUM_MENU_ITEMS - 2;
            } else if (selectedMenuItem < firstVisibleMenuItem) {
                firstVisibleMenuItem = selectedMenuItem;
            }

            Serial.println("UP button pressed");
            Display::drawMenu(selectedMenuItem, firstVisibleMenuItem);
            buttonPressed = true;

        } else if (isButtonPressed(BUTTON_DOWN)) {
            // Wrap around if at the bottom
            selectedMenuItem = static_cast<MenuItem>((selectedMenuItem + 1) % NUM_MENU_ITEMS);

            if (selectedMenuItem == 0) {
                // If wrapped to the top, make it visible
                firstVisibleMenuItem = 0;
            } else if (selectedMenuItem >= (firstVisibleMenuItem + 2)) {
                firstVisibleMenuItem = selectedMenuItem - 1;
            }

            Serial.println("DOWN button pressed");
            Display::drawMenu(selectedMenuItem, firstVisibleMenuItem);
            buttonPressed = true;

        } else if (isButtonPressed(BUTTON_SELECT)) {
            Serial.println("SELECT button pressed");
            // Note: Actual menu item execution will be handled by main loop
            // This just sets the flag
            buttonPressed = true;
        }
    } else {
        // If no button is pressed, reset the buttonPressed flag
        if (!isButtonPressed(BUTTON_UP) &&
            !isButtonPressed(BUTTON_DOWN) &&
            !isButtonPressed(BUTTON_SELECT)) {
            buttonPressed = false;
            digitalWrite(LED_PIN, LOW);  // Turn off LED
        }
    }
}
