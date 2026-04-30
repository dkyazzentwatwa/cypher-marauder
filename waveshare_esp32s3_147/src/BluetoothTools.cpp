#include "BluetoothTools.h"

#include "../include/BoardConfig.h"
#include "SessionLogger.h"

#if ENABLE_BLE_SCAN
#include <NimBLEDevice.h>

namespace {
constexpr uint32_t BLE_SCAN_TOTAL_MS = 5000;
constexpr uint32_t BLE_SCAN_SLICE_MS = 500;
}
#endif

void BluetoothTools::begin() {
#if ENABLE_BLE_SCAN
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  NimBLEScan *scan = NimBLEDevice::getScan();
  scan->setActiveScan(true);
  scan->setInterval(97);
  scan->setWindow(37);
  scan->setDuplicateFilter(1);
#endif
}

void BluetoothTools::startScan() {
  ScanUi::resetState(_uiState);
  _itemCount = 0;
  _scanning = true;
  _scanStartMs = millis();
  Serial.println("BLE scan requested");

#if ENABLE_BLE_SCAN
  _status = "Scanning";
  NimBLEScan *scan = NimBLEDevice::getScan();
  if (scan->isScanning()) {
    scan->stop();
  }
  scan->clearResults();
  startScanSlice();
#else
  _status = "NimBLE disabled";
  _scanning = false;
#endif
}

bool BluetoothTools::startContinuous() {
#if ENABLE_BLE_SCAN
  _continuous = true;
  startScan();
  return true;
#else
  _status = "NimBLE disabled";
  _continuous = false;
  _scanning = false;
  return false;
#endif
}

void BluetoothTools::stopContinuous() {
  _continuous = false;
}

bool BluetoothTools::handleButton(ButtonEvent event) {
  return ScanUi::handleButton(_uiState, _itemCount, event);
}

#if ENABLE_BLE_SCAN
void BluetoothTools::startScanSlice() {
  NimBLEScan *scan = NimBLEDevice::getScan();
  if (!scan->start(BLE_SCAN_SLICE_MS, false, true)) {
    _status = "Scan start failed";
    _scanning = false;
  }
}

void BluetoothTools::harvestSliceResults() {
  NimBLEScan *scan = NimBLEDevice::getScan();
  NimBLEScanResults results = scan->getResults();
  const int count = results.getCount();
  for (int i = 0; i < count; i++) {
    const NimBLEAdvertisedDevice *device = results.getDevice(i);
    ScanItem item;
    item.rssi = device->getRSSI();
    item.connectable = device->isConnectable();
    item.hasTxPower = device->haveTXPower();
    item.txPower = item.hasTxPower ? device->getTXPower() : 0;
    String name = device->getName().c_str();
    String addr = device->getAddress().toString().c_str();
    if (name.length() == 0) {
      name = addr;
    }
    item.name = name;
    item.id = addr;
    SessionLogger::instance().appendBle(item.rssi, item.connectable, item.name, item.id, item.txPower,
                                        item.hasTxPower);
    mergeItem(item);
  }
  scan->clearResults();
}
#endif

void BluetoothTools::update(DisplayPort &display, bool drawToScreen) {
#if ENABLE_BLE_SCAN
  if (_scanning) {
    NimBLEScan *scan = NimBLEDevice::getScan();
    if (!scan->isScanning()) {
      harvestSliceResults();
      if ((millis() - _scanStartMs) < BLE_SCAN_TOTAL_MS) {
        _status = "Scanning";
        startScanSlice();
      } else {
        if (_continuous) {
          _status = "Scanning";
          _scanStartMs = millis();
          _itemCount = 0;
          startScanSlice();
        } else {
          _status = "Done";
          _scanning = false;
        }
      }
    } else {
      _status = "Scanning";
    }
  }

  if (drawToScreen) {
    ScanUi::draw(display, "BLE Scan", _items, _itemCount, _uiState, _scanning, _status, SCAN_SOURCE_BLE);
  }
#else
  if (drawToScreen) {
    ScanUi::draw(display, "BLE Scan", _items, _itemCount, _uiState, false, "NimBLE disabled", SCAN_SOURCE_BLE);
  }
#endif
}

bool BluetoothTools::isScanning() const {
  return _scanning;
}

bool BluetoothTools::isContinuous() const {
  return _continuous;
}

bool BluetoothTools::available() const {
#if ENABLE_BLE_SCAN
  return true;
#else
  return false;
#endif
}

String BluetoothTools::statusText() const {
  return _status;
}

int BluetoothTools::itemCount() const {
  return _itemCount;
}

int BluetoothTools::copyItems(ScanItem *out, int maxCount) const {
  if (out == nullptr || maxCount <= 0) {
    return 0;
  }
  const int count = min(_itemCount, maxCount);
  for (int i = 0; i < count; i++) {
    out[i] = _items[i];
  }
  return count;
}

void BluetoothTools::stop() {
#if ENABLE_BLE_SCAN
  NimBLEScan *scan = NimBLEDevice::getScan();
  if (scan->isScanning()) {
    scan->stop();
  }
#endif
  stopContinuous();
  _status = "Stopped";
  _scanning = false;
}

void BluetoothTools::mergeItem(const ScanItem &item) {
  for (int i = 0; i < _itemCount; i++) {
    if (_items[i].id == item.id) {
      if (item.rssi > _items[i].rssi) {
        _items[i].rssi = item.rssi;
      }
      if (_items[i].name == _items[i].id && item.name != item.id) {
        _items[i].name = item.name;
      }
      _items[i].connectable = _items[i].connectable || item.connectable;
      if (item.hasTxPower) {
        _items[i].hasTxPower = true;
        _items[i].txPower = item.txPower;
      }
      return;
    }
  }

  if (_itemCount < ScanUi::MAX_ITEMS) {
    _items[_itemCount] = item;
    _itemCount++;
  } else if (item.rssi > _items[_itemCount - 1].rssi) {
    _items[_itemCount - 1] = item;
  } else {
    return;
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
