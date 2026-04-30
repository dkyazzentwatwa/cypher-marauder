#include "AppMenu.h"

#include <Adafruit_ST7789.h>

namespace {
struct MenuItem {
  const char *label;
  MenuAction action;
};

constexpr MenuItem MENU_ITEMS[] = {
    {"WiFi scanner", MENU_ACTION_OPEN_WIFI},
    {"BLE scanner", MENU_ACTION_OPEN_BLE},
    {"AP web server", MENU_ACTION_OPEN_AP},
    {"Captive Portal", MENU_ACTION_OPEN_PORTAL},
    {"System info", MENU_ACTION_OPEN_SYSTEM},
    {"USB HID actions", MENU_ACTION_OPEN_USB_HID},
    {"Channel monitor", MENU_ACTION_OPEN_CHANNEL},
    {"Display test", MENU_ACTION_OPEN_DISPLAY_TEST},
    {"Touch test", MENU_ACTION_OPEN_TOUCH_TEST},
    {"Settings/About", MENU_ACTION_OPEN_ABOUT},
};

constexpr int MENU_COUNT = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
constexpr int ROW_HEIGHT = 26;
constexpr int FIRST_ROW_Y = 28;
}

void AppMenu::begin(DisplayPort *display) {
  _display = display;
  _selected = 0;
  _top = 0;
  _lastSelected = -1;
}

MenuAction AppMenu::handleButton(ButtonEvent event, AppScreen currentScreen) {
  if (event == BUTTON_NONE) {
    return MENU_ACTION_NONE;
  }

  if (currentScreen != SCREEN_MENU) {
    if (event == BUTTON_HOLD_5S || event == BUTTON_CLICK || event == BUTTON_BACK) {
      return MENU_ACTION_BACK;
    }
    return MENU_ACTION_NONE;
  }

  if (event == BUTTON_CLICK) {
    _selected = (_selected + 1) % MENU_COUNT;
    const int visibleRows = max(1, (_display->height() - FIRST_ROW_Y - 22) / ROW_HEIGHT);
    if (_selected < _top) {
      _top = _selected;
    } else if (_selected >= _top + visibleRows) {
      _top = _selected - visibleRows + 1;
    }
    draw(SCREEN_MENU, true);
  } else if (event == BUTTON_PREV) {
    _selected = (_selected - 1 + MENU_COUNT) % MENU_COUNT;
    const int visibleRows = max(1, (_display->height() - FIRST_ROW_Y - 22) / ROW_HEIGHT);
    if (_selected < _top) {
      _top = _selected;
    } else if (_selected >= _top + visibleRows) {
      _top = _selected - visibleRows + 1;
    }
    draw(SCREEN_MENU, true);
  } else if (event == BUTTON_SELECT) {
    return selectedAction();
  } else if (event == BUTTON_HOLD_2S) {
    return selectedAction();
  } else if (event == BUTTON_HOLD_5S || event == BUTTON_BACK) {
    return MENU_ACTION_OPEN_ABOUT;
  }

  return MENU_ACTION_NONE;
}

void AppMenu::draw(AppScreen currentScreen, bool force) {
  if (!_display || currentScreen != SCREEN_MENU) {
    return;
  }
  if (!force && _lastScreen == currentScreen && _lastSelected == _selected) {
    return;
  }

  Adafruit_GFX &gfx = _display->gfx();
  _display->clear();
  _display->drawHeader(" CYPHER-DRIVE");
  gfx.drawFastHLine(0, FIRST_ROW_Y - 3, _display->width(), 0x31A6);

  const int visibleRows = max(1, (_display->height() - FIRST_ROW_Y - 22) / ROW_HEIGHT);
  for (int row = 0; row < visibleRows; row++) {
    const int itemIndex = _top + row;
    if (itemIndex >= MENU_COUNT) {
      break;
    }

    const int y = FIRST_ROW_Y + row * ROW_HEIGHT;
    const int cardH = ROW_HEIGHT - 4;
    const bool selected = itemIndex == _selected;
    const uint16_t bg = selected ? ST77XX_CYAN : 0x0841;
    const uint16_t border = selected ? ST77XX_CYAN : 0x4A69;
    const uint16_t fg = selected ? ST77XX_BLACK : ST77XX_WHITE;
    const int cardX = 4;
    const int cardW = _display->width() - 8;

    gfx.drawRoundRect(cardX, y, cardW, cardH, 5, border);
    gfx.fillRoundRect(cardX + 1, y + 1, cardW - 2, cardH - 2, 4, bg);

    const int badgeX = cardX + 5;
    const int badgeY = y + 4;
    const int badgeW = 20;
    const int badgeH = cardH - 8;
    const uint16_t badgeBg = selected ? ST77XX_BLACK : ST77XX_CYAN;
    const uint16_t badgeFg = selected ? ST77XX_CYAN : ST77XX_BLACK;
    gfx.fillRoundRect(badgeX, badgeY, badgeW, badgeH, 3, badgeBg);
    gfx.setTextColor(badgeFg, badgeBg);
    gfx.setCursor(badgeX + 3, badgeY + 3);
    if (itemIndex < 9) {
      gfx.print("0");
    }
    gfx.print(itemIndex + 1);

    gfx.setTextColor(fg, bg);
    gfx.setCursor(cardX + 30, y + 8);
    gfx.print(MENU_ITEMS[itemIndex].label);
  }

  _display->drawFooter("  Click next  Hold select");
  _lastScreen = currentScreen;
  _lastSelected = _selected;
}

void AppMenu::drawMessage(const char *title, const String &body, const char *footer) {
  if (!_display) {
    return;
  }
  _display->drawMessage(title, body, footer);
}

MenuAction AppMenu::selectedAction() const {
  return MENU_ITEMS[_selected].action;
}
