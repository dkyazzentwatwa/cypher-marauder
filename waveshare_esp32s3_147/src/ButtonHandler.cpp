#include "ButtonHandler.h"

namespace {
constexpr uint32_t DEBOUNCE_MS = 30;
constexpr uint32_t HOLD_2S_MS = 2000;
constexpr uint32_t HOLD_5S_MS = 5000;
}

ButtonHandler::ButtonHandler(uint8_t pin, bool activeLow, bool autoDetectPolarity)
    : _pin(pin), _configuredActiveLow(activeLow), _activeLow(activeLow),
      _autoDetectPolarity(autoDetectPolarity) {}

void ButtonHandler::begin() {
  _activeLow = _configuredActiveLow;

  if (_autoDetectPolarity) {
    pinMode(_pin, INPUT);
    delay(2);
    int highCount = 0;
    constexpr int sampleCount = 24;
    for (int i = 0; i < sampleCount; i++) {
      if (digitalRead(_pin) == HIGH) {
        highCount++;
      }
      delayMicroseconds(300);
    }
    const bool idleHigh = highCount > (sampleCount / 2);
    _activeLow = idleHigh;
  }

  pinMode(_pin, _activeLow ? INPUT_PULLUP : INPUT_PULLDOWN);
  _lastRawPressed = readPressed();
  _stablePressed = _lastRawPressed;
  _lastDebounceMs = millis();
}

ButtonEvent ButtonHandler::update() {
  const uint32_t now = millis();
  const bool rawPressed = readPressed();

  if (rawPressed != _lastRawPressed) {
    _lastRawPressed = rawPressed;
    _lastDebounceMs = now;
  }

  if ((now - _lastDebounceMs) < DEBOUNCE_MS) {
    return BUTTON_NONE;
  }

  if (rawPressed != _stablePressed) {
    _stablePressed = rawPressed;
    if (_stablePressed) {
      _pressStartMs = now;
      _suppressClick = false;
      _hold2Sent = false;
      _hold5Sent = false;
    } else {
      if (!_suppressClick) {
        return BUTTON_CLICK;
      }
    }
  }

  if (_stablePressed) {
    const uint32_t heldMs = now - _pressStartMs;
    if (!_hold5Sent && heldMs >= HOLD_5S_MS) {
      _hold5Sent = true;
      _suppressClick = true;
      return BUTTON_HOLD_5S;
    }
    if (!_hold2Sent && heldMs >= HOLD_2S_MS) {
      _hold2Sent = true;
      _suppressClick = true;
      return BUTTON_HOLD_2S;
    }
  }

  return BUTTON_NONE;
}

bool ButtonHandler::isPressed() const {
  return _stablePressed;
}

uint32_t ButtonHandler::pressedDurationMs(uint32_t nowMs) const {
  if (!_stablePressed) {
    return 0;
  }
  return nowMs - _pressStartMs;
}

bool ButtonHandler::readPressed() const {
  const int value = digitalRead(_pin);
  return _activeLow ? (value == LOW) : (value == HIGH);
}
