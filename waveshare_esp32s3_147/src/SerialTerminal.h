#pragma once

#include <Arduino.h>

#include "AppMenu.h"
#include "BluetoothTools.h"
#include "CaptivePortal.h"
#include "HidActions.h"
#include "WebMode.h"
#include "WifiTools.h"

struct SerialTerminalContext {
  WifiTools *wifiTools = nullptr;
  BluetoothTools *bluetoothTools = nullptr;
  WebMode *webMode = nullptr;
  HidActions *hidActions = nullptr;
  CaptivePortal *captivePortal = nullptr;

  AppScreen (*getCurrentScreen)() = nullptr;
  void (*openScreen)(AppScreen screen) = nullptr;
  void (*goBackToMenu)() = nullptr;
  void (*handleMenuAction)(MenuAction action) = nullptr;

  void (*channelStart)() = nullptr;
  void (*channelStop)() = nullptr;
  bool (*channelIsActive)() = nullptr;
  bool (*channelIsScanning)() = nullptr;
  uint32_t (*channelLastScanAgeSec)() = nullptr;
  void (*channelCopyCounts)(int *outCounts, int maxCount) = nullptr;
};

class SerialTerminal {
public:
  void begin(const SerialTerminalContext &ctx);
  void poll();

private:
  static constexpr size_t MAX_LINE_LEN = 192;
  static constexpr uint8_t MAX_TOKENS = 12;

  struct ParsedLine {
    String tokens[MAX_TOKENS];
    uint8_t count = 0;
  };

  void printPrompt();
  void printHelp(const String &topic);
  void printStatus();
  void handleLine(const String &line);
  ParsedLine parseTokens(const String &line) const;
  bool parseOnOff(const String &token, bool &value) const;
  bool parseU8(const String &token, uint8_t &value) const;
  bool same(const String &a, const char *b) const;
  String screenName(AppScreen screen) const;
  bool openScreenByName(const String &name);
  void printScanItems(const char *title, const ScanItem *items, int count, bool isWifi) const;
  void printHidList(const String &categoryArg);
  void printChannelSnapshot();
  void printUnknown(const String &cmd);
  void printOk(const String &msg) const;
  void printErr(const String &msg) const;

  SerialTerminalContext _ctx;
  char _lineBuf[MAX_LINE_LEN] = {0};
  size_t _lineLen = 0;
  bool _sawCarriageReturn = false;
};
