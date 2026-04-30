#pragma once

#include "ButtonHandler.h"

class MultiButtonInput {
public:
  MultiButtonInput(uint8_t upPin, uint8_t selectPin, uint8_t backPin, bool activeLow);
  void begin();
  ButtonEvent update();
  bool isBackPressed() const;

private:
  ButtonHandler _up;
  ButtonHandler _select;
  ButtonHandler _back;
};
