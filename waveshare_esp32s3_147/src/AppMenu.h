#pragma once

#include <Arduino.h>

#include "ButtonHandler.h"
#include "DisplayPort.h"

enum AppScreen {
  SCREEN_MENU,
  SCREEN_WIFI_SCAN,
  SCREEN_BLE_SCAN,
  SCREEN_AP_SERVER,
  SCREEN_SYSTEM_INFO,
  SCREEN_USB_HID,
  SCREEN_CHANNEL_MONITOR,
  SCREEN_DISPLAY_TEST,
  SCREEN_TOUCH_TEST,
  SCREEN_ABOUT,
  SCREEN_PORTAL
};

enum MenuAction {
  MENU_ACTION_NONE,
  MENU_ACTION_OPEN_WIFI,
  MENU_ACTION_OPEN_BLE,
  MENU_ACTION_OPEN_AP,
  MENU_ACTION_OPEN_SYSTEM,
  MENU_ACTION_OPEN_USB_HID,
  MENU_ACTION_OPEN_CHANNEL,
  MENU_ACTION_OPEN_DISPLAY_TEST,
  MENU_ACTION_OPEN_TOUCH_TEST,
  MENU_ACTION_OPEN_ABOUT,
  MENU_ACTION_OPEN_PORTAL,
  MENU_ACTION_BACK
};

class AppMenu {
public:
  void begin(DisplayPort *display);
  MenuAction handleButton(ButtonEvent event, AppScreen currentScreen);
  void draw(AppScreen currentScreen, bool force = false);
  void drawMessage(const char *title, const String &body, const char *footer);

private:
  MenuAction selectedAction() const;

  DisplayPort *_display = nullptr;
  int _selected = 0;
  int _top = 0;
  AppScreen _lastScreen = SCREEN_MENU;
  int _lastSelected = -1;
};
