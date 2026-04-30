#include "MultiButtonInput.h"

MultiButtonInput::MultiButtonInput(uint8_t upPin, uint8_t selectPin, uint8_t backPin, bool activeLow)
    : _up(upPin, activeLow, false), _select(selectPin, activeLow, false), _back(backPin, activeLow, false) {}

void MultiButtonInput::begin() {
  _up.begin();
  _select.begin();
  _back.begin();
}

ButtonEvent MultiButtonInput::update() {
  const ButtonEvent backEvent = _back.update();
  if (backEvent == BUTTON_HOLD_5S) {
    return BUTTON_HOLD_5S;
  }
  if (backEvent == BUTTON_CLICK) {
    return BUTTON_BACK;
  }

  const ButtonEvent selectEvent = _select.update();
  if (selectEvent == BUTTON_CLICK) {
    return BUTTON_SELECT;
  }
  if (selectEvent == BUTTON_HOLD_2S) {
    return BUTTON_HOLD_2S;
  }

  const ButtonEvent upEvent = _up.update();
  if (upEvent == BUTTON_CLICK || upEvent == BUTTON_HOLD_2S) {
    return BUTTON_PREV;
  }

  return BUTTON_NONE;
}

bool MultiButtonInput::isBackPressed() const {
  return _back.isPressed();
}
