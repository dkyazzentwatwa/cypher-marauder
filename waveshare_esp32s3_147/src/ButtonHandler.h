#pragma once

#include <Arduino.h>

enum ButtonEvent {
  BUTTON_NONE,
  BUTTON_CLICK,
  BUTTON_HOLD_2S,
  BUTTON_HOLD_5S,
  BUTTON_PREV,
  BUTTON_SELECT,
  BUTTON_BACK
};

class ButtonHandler {
public:
  ButtonHandler(uint8_t pin, bool activeLow, bool autoDetectPolarity);
  void begin();
  ButtonEvent update();
  bool isPressed() const;
  uint32_t pressedDurationMs(uint32_t nowMs) const;

private:
  bool readPressed() const;

  uint8_t _pin;
  bool _configuredActiveLow;
  bool _activeLow;
  bool _autoDetectPolarity;
  bool _stablePressed = false;
  bool _lastRawPressed = false;
  bool _suppressClick = false;
  bool _hold2Sent = false;
  bool _hold5Sent = false;
  uint32_t _lastDebounceMs = 0;
  uint32_t _pressStartMs = 0;
};
