#pragma once

#include <Arduino.h>

#include "ButtonHandler.h"
#include "DisplayPort.h"

enum ScanSource {
  SCAN_SOURCE_WIFI,
  SCAN_SOURCE_BLE
};

struct ScanItem {
  String name;
  String id;
  int rssi = -127;
  int channel = 0;
  bool hasChannel = false;
  bool secure = false;
  bool connectable = false;
  bool hasTxPower = false;
  int txPower = 0;
};

struct ScanUiState {
  int selected = 0;
  bool detailMode = false;
  bool painted = false;
  uint32_t lastHash = 0;
};

class ScanUi {
public:
  static constexpr int MAX_ITEMS = 40;

  static void resetState(ScanUiState &state);
  static bool handleButton(ScanUiState &state, int itemCount, ButtonEvent event);
  static void draw(DisplayPort &display, const char *title, const ScanItem *items, int itemCount,
                   ScanUiState &state, bool scanning, const String &status, ScanSource source);

private:
  static String truncateText(const String &in, int maxChars);
  static int rowsPerPage(const DisplayPort &display);
  static int pageForIndex(const DisplayPort &display, int index);
};
