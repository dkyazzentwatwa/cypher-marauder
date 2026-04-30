#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

#include "BluetoothTools.h"
#include "DisplayPort.h"
#include "WifiTools.h"

class WebMode {
public:
  enum ActiveRadio {
    RADIO_NONE,
    RADIO_WIFI,
    RADIO_BLE
  };

  void attachScanners(WifiTools *wifiTools, BluetoothTools *bluetoothTools);
  void begin();
  void poll();
  void update(DisplayPort &display);
  void stop();
  bool active() const;
  ActiveRadio activeRadio() const;

private:
  bool requireJsonPost();
  String jsonEscape(const String &in) const;
  const char *activeRadioString() const;
  String buildStatusJson() const;
  void refreshStatusCache(bool force);
  void stopAllScans();
  bool startWifiContinuous(String &message, int &httpCode);
  bool stopWifiContinuous(String &message, int &httpCode);
  bool startBleContinuous(String &message, int &httpCode);
  bool stopBleContinuous(String &message, int &httpCode);
  void configureRoutes();

  WebServer _server{80};
  WifiTools *_wifiTools = nullptr;
  BluetoothTools *_bluetoothTools = nullptr;
  ActiveRadio _activeRadio = RADIO_NONE;
  bool _active = false;
  bool _painted = false;
  String _statusJsonCache;
  uint32_t _lastStatusBuildMs = 0;
  static constexpr uint32_t STATUS_CACHE_INTERVAL_MS = 250;
};
