#include "WifiTools.h"

#include <Adafruit_ST77xx.h>

#include "SessionLogger.h"

void WifiTools::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false, false);
}

void WifiTools::startScan() {
  _continuous = false;
  startAsyncScan();
}

void WifiTools::startContinuous() {
  _continuous = true;
  startAsyncScan();
}

void WifiTools::stopContinuous() {
  _continuous = false;
  _nextScanAtMs = 0;
}

void WifiTools::startAsyncScan() {
  // Keep AP alive when scans are launched from AP web mode.
  const wifi_mode_t mode = WiFi.getMode();
  if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA) {
    WiFi.mode(WIFI_AP_STA);
  } else {
    WiFi.mode(WIFI_STA);
  }
  WiFi.scanDelete();
  ScanUi::resetState(_uiState);
  _itemCount = 0;
  _hasResults = false;
  _status = "scanning";
  _lastPaintMs = 0;
  const int startResult = WiFi.scanNetworks(true, true);
  if (startResult == WIFI_SCAN_FAILED) {
    _status = "scan_start_failed";
    _scanning = false;
    _continuous = false;
    Serial.println("WiFi async scan start failed");
    return;
  }
  _scanning = true;
  Serial.printf("WiFi async scan started result=%d\n", startResult);
}

bool WifiTools::handleButton(ButtonEvent event) {
  return ScanUi::handleButton(_uiState, _itemCount, event);
}

void WifiTools::update(DisplayPort &display, bool drawToScreen) {
  const int result = WiFi.scanComplete();
  const uint32_t now = millis();

  if (_continuous && !_scanning && now >= _nextScanAtMs) {
    startAsyncScan();
  }

  if (result == WIFI_SCAN_RUNNING) {
    if (drawToScreen && now - _lastPaintMs > 250) {
      _status = "scanning";
      ScanUi::draw(display, "WiFi Scan", _items, _itemCount, _uiState, true, _status, SCAN_SOURCE_WIFI);
      _lastPaintMs = now;
    }
    return;
  }

  if (result < 0) {
    if (_scanning) {
      _status = "scan_failed";
      if (drawToScreen) {
        ScanUi::draw(display, "WiFi Scan", _items, _itemCount, _uiState, false, _status, SCAN_SOURCE_WIFI);
      }
      _scanning = false;
      if (_continuous) {
        _status = "scanning";
        _nextScanAtMs = now + 250;
      }
      Serial.printf("WiFi scan failed result=%d\n", result);
    }
    return;
  }

  if (_scanning || !_hasResults) {
    _itemCount = 0;
    for (int i = 0; i < result; i++) {
      ScanItem item;
      item.rssi = WiFi.RSSI(i);
      item.channel = WiFi.channel(i);
      item.hasChannel = true;
      item.secure = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;

      String ssid = WiFi.SSID(i);
      if (ssid.length() == 0) {
        ssid = "<hidden>";
      }
      item.name = ssid;
      item.id = WiFi.BSSIDstr(i);
      SessionLogger::instance().appendWifi(item.rssi, item.channel, item.secure, item.name, item.id);
      addSortedItem(item);
    }
    _hasResults = true;
    _scanning = false;
    if (_continuous) {
      _status = "scanning";
      _nextScanAtMs = now + 250;
    } else {
      _status = _itemCount == 0 ? "no_results" : "done";
      _nextScanAtMs = 0;
    }
    Serial.printf("WiFi scan done: %d networks\n", result);
    WiFi.scanDelete();
  }

  if (drawToScreen) {
    ScanUi::draw(display, "WiFi Scan", _items, _itemCount, _uiState, _scanning, _status, SCAN_SOURCE_WIFI);
  }
}

bool WifiTools::isScanning() const {
  return _scanning;
}

bool WifiTools::isContinuous() const {
  return _continuous;
}

String WifiTools::statusText() const {
  return _status;
}

int WifiTools::itemCount() const {
  return _itemCount;
}

int WifiTools::copyItems(ScanItem *out, int maxCount) const {
  if (out == nullptr || maxCount <= 0) {
    return 0;
  }
  const int count = min(_itemCount, maxCount);
  for (int i = 0; i < count; i++) {
    out[i] = _items[i];
  }
  return count;
}

void WifiTools::stop() {
  WiFi.scanDelete();
  stopContinuous();
  _status = "Stopped";
  _scanning = false;
}

void WifiTools::addSortedItem(const ScanItem &item) {
  if (_itemCount < ScanUi::MAX_ITEMS) {
    _items[_itemCount] = item;
    _itemCount++;
  } else if (item.rssi <= _items[_itemCount - 1].rssi) {
    return;
  } else {
    _items[_itemCount - 1] = item;
  }

  for (int i = _itemCount - 1; i > 0; i--) {
    if (_items[i].rssi > _items[i - 1].rssi) {
      ScanItem tmp = _items[i - 1];
      _items[i - 1] = _items[i];
      _items[i] = tmp;
    } else {
      break;
    }
  }
}
