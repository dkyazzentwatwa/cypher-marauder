#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "ButtonHandler.h"
#include "DisplayPort.h"
#include "ScanUi.h"

class WifiTools {
public:
  void begin();
  void startScan();
  void startContinuous();
  void stopContinuous();
  bool handleButton(ButtonEvent event);
  void update(DisplayPort &display, bool drawToScreen);
  bool isScanning() const;
  bool isContinuous() const;
  String statusText() const;
  int itemCount() const;
  int copyItems(ScanItem *out, int maxCount) const;
  void stop();

private:
  void startAsyncScan();
  void addSortedItem(const ScanItem &item);

  ScanItem _items[ScanUi::MAX_ITEMS];
  int _itemCount = 0;
  bool _hasResults = false;
  ScanUiState _uiState;
  String _status;
  bool _scanning = false;
  bool _continuous = false;
  uint32_t _nextScanAtMs = 0;
  uint32_t _lastPaintMs = 0;
};
