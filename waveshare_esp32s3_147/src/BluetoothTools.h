#pragma once

#include <Arduino.h>

#include "ButtonHandler.h"
#include "DisplayPort.h"
#include "ScanUi.h"

class BluetoothTools {
public:
  void begin();
  void startScan();
  bool startContinuous();
  void stopContinuous();
  bool handleButton(ButtonEvent event);
  void update(DisplayPort &display, bool drawToScreen);
  bool isScanning() const;
  bool isContinuous() const;
  bool available() const;
  String statusText() const;
  int itemCount() const;
  int copyItems(ScanItem *out, int maxCount) const;
  void stop();

private:
  void startScanSlice();
  void harvestSliceResults();
  void mergeItem(const ScanItem &item);

  ScanItem _items[ScanUi::MAX_ITEMS];
  int _itemCount = 0;
  ScanUiState _uiState;
  String _status = "Idle";
  uint32_t _scanStartMs = 0;

  bool _scanning = false;
  bool _continuous = false;
};
