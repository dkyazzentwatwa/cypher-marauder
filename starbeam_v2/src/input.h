// input.h - Input Handling Module for Project Starbeam V2
// Phase 1: Extract V1 logic (with blocking delays - will optimize in Phase 2)

#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include "../config.h"
#include "../types.h"

class Input {
public:
    // Initialization
    static void init();

    // Button checking (V1 style - blocking)
    static bool isButtonPressed(uint8_t pin);

    // Menu selection handler
    static void handleMenuSelection(MenuItem& selectedMenuItem, int& firstVisibleMenuItem);

    // Get current button state
    static bool getButtonPressedState() { return buttonPressed; }
    static void setButtonPressedState(bool state) { buttonPressed = state; }

private:
    static bool buttonPressed;
};

#endif // INPUT_H
