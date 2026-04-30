#include "SerialTerminal.h"

#include <ctype.h>
#include "MarauderCore.h"

namespace {
const char *kScreenNames[] = {
    "menu", "wifi", "ble", "ap", "system", "hid", "channel", "display", "touch", "about", "portal", "marauder",
};
}

void SerialTerminal::begin(const SerialTerminalContext &ctx) {
  _ctx = ctx;
  _lineLen = 0;
  _sawCarriageReturn = false;
  memset(_lineBuf, 0, sizeof(_lineBuf));
  Serial.println("serial terminal ready; type 'help'");
  MarauderCore::core().begin("waveshare-esp32s3-147");
  printPrompt();
}

void SerialTerminal::poll() {
  MarauderCore::core().poll();

  while (Serial.available() > 0) {
    const int raw = Serial.read();
    if (raw < 0) {
      break;
    }
    const char ch = static_cast<char>(raw);
    if (ch == '\r') {
      _sawCarriageReturn = true;
      _lineBuf[_lineLen] = '\0';
      const String line = String(_lineBuf);
      _lineLen = 0;
      _lineBuf[0] = '\0';
      if (line.length() > 0) {
        handleLine(line);
      } else {
        printPrompt();
      }
      continue;
    }
    if (ch == '\n') {
      if (_sawCarriageReturn) {
        _sawCarriageReturn = false;
        continue;
      }
      _lineBuf[_lineLen] = '\0';
      const String line = String(_lineBuf);
      _lineLen = 0;
      _lineBuf[0] = '\0';
      if (line.length() > 0) {
        handleLine(line);
      } else {
        printPrompt();
      }
      continue;
    }
    _sawCarriageReturn = false;
    if (ch == '\b' || ch == 127) {
      if (_lineLen > 0) {
        _lineLen--;
        _lineBuf[_lineLen] = '\0';
      }
      continue;
    }
    if (_lineLen + 1 < MAX_LINE_LEN && isprint(static_cast<unsigned char>(ch))) {
      _lineBuf[_lineLen++] = ch;
      _lineBuf[_lineLen] = '\0';
    }
  }
}

void SerialTerminal::printPrompt() {
  Serial.print("cli> ");
}

void SerialTerminal::printHelp(const String &topicRaw) {
  String topic = topicRaw;
  topic.toLowerCase();
  if (topic.length() == 0 || topic == "all") {
    Serial.println("commands:");
    Serial.println("  help [topic], status");
    Serial.println("  screen list|show|open <name>|menu");
    Serial.println("  wifi start|continuous on|continuous off|stop|status|list");
    Serial.println("  ble start|continuous on|continuous off|stop|status|list");
    Serial.println("  ap start|stop|status");
    Serial.println("  portal start|stop|status");
    Serial.println("  hid status|list [category]|run <index>|stop");
    Serial.println("  channel start|stop|status|snapshot");
    Serial.println("  system info");
    Serial.println("  about");
    Serial.println("  display test");
    Serial.println("  touch test");
    return;
  }
  if (topic == "screen") {
    Serial.println("screen names: menu wifi ble ap system hid channel display touch about portal marauder");
    return;
  }
  if (topic == "hid") {
    Serial.println("hid list [category], hid run <index>, hid stop");
    Serial.println("indexes are 1-based inside selected category");
    return;
  }
  Serial.println("no help for that topic");
}

void SerialTerminal::printStatus() {
  const AppScreen screen = _ctx.getCurrentScreen ? _ctx.getCurrentScreen() : SCREEN_MENU;
  Serial.printf("screen=%s\n", screenName(screen).c_str());
  if (_ctx.wifiTools) {
    Serial.printf("wifi scanning=%d continuous=%d status=\"%s\" items=%d\n", _ctx.wifiTools->isScanning() ? 1 : 0,
                  _ctx.wifiTools->isContinuous() ? 1 : 0, _ctx.wifiTools->statusText().c_str(), _ctx.wifiTools->itemCount());
  }
  if (_ctx.bluetoothTools) {
    Serial.printf("ble scanning=%d continuous=%d status=\"%s\" items=%d\n",
                  _ctx.bluetoothTools->isScanning() ? 1 : 0, _ctx.bluetoothTools->isContinuous() ? 1 : 0,
                  _ctx.bluetoothTools->statusText().c_str(), _ctx.bluetoothTools->itemCount());
  }
  if (_ctx.webMode) {
    Serial.printf("ap active=%d radio=%d\n", _ctx.webMode->active() ? 1 : 0, static_cast<int>(_ctx.webMode->activeRadio()));
  }
  if (_ctx.captivePortal) {
    Serial.printf("portal active=%d\n", _ctx.captivePortal->active() ? 1 : 0);
  }
  if (_ctx.hidActions) {
    Serial.printf("hid available=%d running=%d category=\"%s\" status=\"%s\"\n", _ctx.hidActions->available() ? 1 : 0,
                  _ctx.hidActions->isRunning() ? 1 : 0, _ctx.hidActions->selectedCategoryName().c_str(),
                  _ctx.hidActions->lastStatus().c_str());
  }
  if (_ctx.channelIsActive && _ctx.channelIsScanning && _ctx.channelLastScanAgeSec) {
    Serial.printf("channel active=%d scanning=%d age_sec=%lu\n", _ctx.channelIsActive() ? 1 : 0,
                  _ctx.channelIsScanning() ? 1 : 0, static_cast<unsigned long>(_ctx.channelLastScanAgeSec()));
  }
}

void SerialTerminal::handleLine(const String &line) {
  ParsedLine parsed = parseTokens(line);
  if (parsed.count == 0) {
    printPrompt();
    return;
  }

  if (MarauderCore::core().handleCommand(line)) {
    printPrompt();
    return;
  }

  const String cmd = parsed.tokens[0];
  if (same(cmd, "help")) {
    printHelp(parsed.count >= 2 ? parsed.tokens[1] : "");
    printPrompt();
    return;
  }
  if (same(cmd, "status")) {
    printStatus();
    printPrompt();
    return;
  }
  if (same(cmd, "screen")) {
    if (parsed.count < 2) {
      printErr("usage: screen list|show|open <name>|menu");
    } else if (same(parsed.tokens[1], "list")) {
      printHelp("screen");
    } else if (same(parsed.tokens[1], "show")) {
      Serial.printf("screen=%s\n", screenName(_ctx.getCurrentScreen ? _ctx.getCurrentScreen() : SCREEN_MENU).c_str());
    } else if (same(parsed.tokens[1], "menu")) {
      if (_ctx.goBackToMenu) {
        _ctx.goBackToMenu();
        printOk("screen menu");
      }
    } else if (same(parsed.tokens[1], "open") && parsed.count >= 3) {
      if (openScreenByName(parsed.tokens[2])) {
        printOk("screen opened");
      } else {
        printErr("unknown screen name");
      }
    } else {
      printErr("usage: screen list|show|open <name>|menu");
    }
    printPrompt();
    return;
  }

  if (same(cmd, "wifi") && _ctx.wifiTools) {
    if (parsed.count < 2) {
      printErr("usage: wifi start|continuous on|continuous off|stop|status|list");
    } else if (same(parsed.tokens[1], "start")) {
      if (_ctx.handleMenuAction) {
        _ctx.handleMenuAction(MENU_ACTION_OPEN_WIFI);
      } else {
        _ctx.wifiTools->startScan();
        if (_ctx.openScreen) {
          _ctx.openScreen(SCREEN_WIFI_SCAN);
        }
      }
      printOk("wifi scan started");
    } else if (same(parsed.tokens[1], "continuous") && parsed.count >= 3) {
      bool on = false;
      if (!parseOnOff(parsed.tokens[2], on)) {
        printErr("continuous expects on|off");
      } else {
        if (on) {
          if (_ctx.handleMenuAction) {
            _ctx.handleMenuAction(MENU_ACTION_OPEN_WIFI);
          }
          _ctx.wifiTools->startContinuous();
          printOk("wifi continuous on");
        } else {
          _ctx.wifiTools->stopContinuous();
          printOk("wifi continuous off");
        }
      }
    } else if (same(parsed.tokens[1], "stop")) {
      _ctx.wifiTools->stop();
      printOk("wifi stopped");
    } else if (same(parsed.tokens[1], "status")) {
      Serial.printf("wifi scanning=%d continuous=%d status=\"%s\" items=%d\n", _ctx.wifiTools->isScanning() ? 1 : 0,
                    _ctx.wifiTools->isContinuous() ? 1 : 0, _ctx.wifiTools->statusText().c_str(), _ctx.wifiTools->itemCount());
    } else if (same(parsed.tokens[1], "list")) {
      ScanItem items[ScanUi::MAX_ITEMS];
      const int count = _ctx.wifiTools->copyItems(items, ScanUi::MAX_ITEMS);
      printScanItems("wifi", items, count, true);
    } else {
      printErr("unknown wifi subcommand");
    }
    printPrompt();
    return;
  }

  if (same(cmd, "ble") && _ctx.bluetoothTools) {
    if (parsed.count < 2) {
      printErr("usage: ble start|continuous on|continuous off|stop|status|list");
    } else if (same(parsed.tokens[1], "start")) {
      if (_ctx.handleMenuAction) {
        _ctx.handleMenuAction(MENU_ACTION_OPEN_BLE);
      } else {
        _ctx.bluetoothTools->startScan();
        if (_ctx.openScreen) {
          _ctx.openScreen(SCREEN_BLE_SCAN);
        }
      }
      printOk("ble scan started");
    } else if (same(parsed.tokens[1], "continuous") && parsed.count >= 3) {
      bool on = false;
      if (!parseOnOff(parsed.tokens[2], on)) {
        printErr("continuous expects on|off");
      } else if (on) {
        if (_ctx.handleMenuAction) {
          _ctx.handleMenuAction(MENU_ACTION_OPEN_BLE);
        }
        const bool started = _ctx.bluetoothTools->startContinuous();
        Serial.printf("%s ble continuous on\n", started ? "ok:" : "error:");
      } else {
        _ctx.bluetoothTools->stopContinuous();
        printOk("ble continuous off");
      }
    } else if (same(parsed.tokens[1], "stop")) {
      _ctx.bluetoothTools->stop();
      printOk("ble stopped");
    } else if (same(parsed.tokens[1], "status")) {
      Serial.printf("ble scanning=%d continuous=%d status=\"%s\" items=%d\n", _ctx.bluetoothTools->isScanning() ? 1 : 0,
                    _ctx.bluetoothTools->isContinuous() ? 1 : 0, _ctx.bluetoothTools->statusText().c_str(),
                    _ctx.bluetoothTools->itemCount());
    } else if (same(parsed.tokens[1], "list")) {
      ScanItem items[ScanUi::MAX_ITEMS];
      const int count = _ctx.bluetoothTools->copyItems(items, ScanUi::MAX_ITEMS);
      printScanItems("ble", items, count, false);
    } else {
      printErr("unknown ble subcommand");
    }
    printPrompt();
    return;
  }

  if (same(cmd, "ap") && _ctx.webMode) {
    if (parsed.count < 2) {
      printErr("usage: ap start|stop|status");
    } else if (same(parsed.tokens[1], "start")) {
      if (_ctx.handleMenuAction) {
        _ctx.handleMenuAction(MENU_ACTION_OPEN_AP);
      } else {
        _ctx.webMode->begin();
        if (_ctx.openScreen) {
          _ctx.openScreen(SCREEN_AP_SERVER);
        }
      }
      printOk("ap started");
    } else if (same(parsed.tokens[1], "stop")) {
      _ctx.webMode->stop();
      printOk("ap stopped");
    } else if (same(parsed.tokens[1], "status")) {
      Serial.printf("ap active=%d radio=%d\n", _ctx.webMode->active() ? 1 : 0, static_cast<int>(_ctx.webMode->activeRadio()));
    } else {
      printErr("unknown ap subcommand");
    }
    printPrompt();
    return;
  }

  if (same(cmd, "portal") && _ctx.captivePortal) {
    if (parsed.count < 2) {
      printErr("usage: portal start|stop|status");
    } else if (same(parsed.tokens[1], "start")) {
      if (_ctx.handleMenuAction) {
        _ctx.handleMenuAction(MENU_ACTION_OPEN_PORTAL);
      } else {
        _ctx.captivePortal->begin();
        if (_ctx.openScreen) {
          _ctx.openScreen(SCREEN_PORTAL);
        }
      }
      printOk("portal started");
    } else if (same(parsed.tokens[1], "stop")) {
      _ctx.captivePortal->stop();
      printOk("portal stopped");
    } else if (same(parsed.tokens[1], "status")) {
      Serial.printf("portal active=%d\n", _ctx.captivePortal->active() ? 1 : 0);
    } else {
      printErr("unknown portal subcommand");
    }
    printPrompt();
    return;
  }

  if (same(cmd, "hid") && _ctx.hidActions) {
    if (parsed.count < 2) {
      printErr("usage: hid status|list [category]|run <index>|stop");
    } else if (same(parsed.tokens[1], "status")) {
      Serial.printf("hid available=%d running=%d category=\"%s\" status=\"%s\"\n", _ctx.hidActions->available() ? 1 : 0,
                    _ctx.hidActions->isRunning() ? 1 : 0, _ctx.hidActions->selectedCategoryName().c_str(),
                    _ctx.hidActions->lastStatus().c_str());
    } else if (same(parsed.tokens[1], "list")) {
      printHidList(parsed.count >= 3 ? parsed.tokens[2] : "");
    } else if (same(parsed.tokens[1], "run") && parsed.count >= 3) {
      uint8_t idx = 0;
      if (!parseU8(parsed.tokens[2], idx) || idx == 0) {
        printErr("hid run expects 1-based index");
      } else if (!_ctx.hidActions->selectPayloadByVisibleIndex(idx - 1)) {
        printErr("payload index out of range");
      } else {
        if (_ctx.handleMenuAction) {
          _ctx.handleMenuAction(MENU_ACTION_OPEN_USB_HID);
        } else if (_ctx.openScreen) {
          _ctx.openScreen(SCREEN_USB_HID);
        }
        const bool started = _ctx.hidActions->runSelected();
        Serial.printf("%s hid run %u\n", started ? "ok:" : "error:", static_cast<unsigned>(idx));
      }
    } else if (same(parsed.tokens[1], "stop")) {
      _ctx.hidActions->stop();
      printOk("hid stopped");
    } else {
      printErr("unknown hid subcommand");
    }
    printPrompt();
    return;
  }

  if (same(cmd, "channel")) {
    if (parsed.count < 2) {
      printErr("usage: channel start|stop|status|snapshot");
    } else if (same(parsed.tokens[1], "start")) {
      if (_ctx.channelStart) {
        _ctx.channelStart();
      }
      if (_ctx.openScreen) {
        _ctx.openScreen(SCREEN_CHANNEL_MONITOR);
      }
      printOk("channel monitor started");
    } else if (same(parsed.tokens[1], "stop")) {
      if (_ctx.channelStop) {
        _ctx.channelStop();
      }
      printOk("channel monitor stopped");
    } else if (same(parsed.tokens[1], "status")) {
      if (_ctx.channelIsActive && _ctx.channelIsScanning && _ctx.channelLastScanAgeSec) {
        Serial.printf("channel active=%d scanning=%d age_sec=%lu\n", _ctx.channelIsActive() ? 1 : 0,
                      _ctx.channelIsScanning() ? 1 : 0, static_cast<unsigned long>(_ctx.channelLastScanAgeSec()));
      } else {
        printErr("channel hooks unavailable");
      }
    } else if (same(parsed.tokens[1], "snapshot")) {
      printChannelSnapshot();
    } else {
      printErr("unknown channel subcommand");
    }
    printPrompt();
    return;
  }

  if (same(cmd, "system") && parsed.count >= 2 && same(parsed.tokens[1], "info")) {
    if (_ctx.handleMenuAction) {
      _ctx.handleMenuAction(MENU_ACTION_OPEN_SYSTEM);
    }
    printOk("system info opened");
    printPrompt();
    return;
  }
  if (same(cmd, "about")) {
    if (_ctx.handleMenuAction) {
      _ctx.handleMenuAction(MENU_ACTION_OPEN_ABOUT);
    }
    printOk("about opened");
    printPrompt();
    return;
  }
  if (same(cmd, "display") && parsed.count >= 2 && same(parsed.tokens[1], "test")) {
    if (_ctx.handleMenuAction) {
      _ctx.handleMenuAction(MENU_ACTION_OPEN_DISPLAY_TEST);
    }
    printOk("display test opened");
    printPrompt();
    return;
  }
  if (same(cmd, "touch") && parsed.count >= 2 && same(parsed.tokens[1], "test")) {
    if (_ctx.handleMenuAction) {
      _ctx.handleMenuAction(MENU_ACTION_OPEN_TOUCH_TEST);
    }
    printOk("touch test opened");
    printPrompt();
    return;
  }

  printUnknown(cmd);
  printPrompt();
}

SerialTerminal::ParsedLine SerialTerminal::parseTokens(const String &line) const {
  ParsedLine out;
  String token;
  for (size_t i = 0; i < line.length(); i++) {
    const char c = line[i];
    if (isspace(static_cast<unsigned char>(c))) {
      if (token.length() > 0 && out.count < MAX_TOKENS) {
        out.tokens[out.count++] = token;
        token = "";
      }
    } else {
      token += c;
    }
  }
  if (token.length() > 0 && out.count < MAX_TOKENS) {
    out.tokens[out.count++] = token;
  }
  return out;
}

bool SerialTerminal::parseOnOff(const String &token, bool &value) const {
  if (same(token, "on")) {
    value = true;
    return true;
  }
  if (same(token, "off")) {
    value = false;
    return true;
  }
  return false;
}

bool SerialTerminal::parseU8(const String &token, uint8_t &value) const {
  char *end = nullptr;
  long parsed = strtol(token.c_str(), &end, 10);
  if (end == token.c_str() || *end != '\0' || parsed < 0 || parsed > 255) {
    return false;
  }
  value = static_cast<uint8_t>(parsed);
  return true;
}

bool SerialTerminal::same(const String &a, const char *b) const {
  return a.equalsIgnoreCase(b);
}

String SerialTerminal::screenName(AppScreen screen) const {
  const int idx = static_cast<int>(screen);
  if (idx < 0 || idx >= static_cast<int>(sizeof(kScreenNames) / sizeof(kScreenNames[0]))) {
    return "unknown";
  }
  return String(kScreenNames[idx]);
}

bool SerialTerminal::openScreenByName(const String &name) {
  if (!_ctx.openScreen || !_ctx.handleMenuAction) {
    return false;
  }
  if (same(name, "menu")) {
    if (_ctx.goBackToMenu) {
      _ctx.goBackToMenu();
    }
    return true;
  }
  if (same(name, "wifi")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_WIFI);
    return true;
  }
  if (same(name, "ble")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_BLE);
    return true;
  }
  if (same(name, "ap")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_AP);
    return true;
  }
  if (same(name, "system")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_SYSTEM);
    return true;
  }
  if (same(name, "hid")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_USB_HID);
    return true;
  }
  if (same(name, "channel")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_CHANNEL);
    return true;
  }
  if (same(name, "display")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_DISPLAY_TEST);
    return true;
  }
  if (same(name, "touch")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_TOUCH_TEST);
    return true;
  }
  if (same(name, "about")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_ABOUT);
    return true;
  }
  if (same(name, "portal")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_PORTAL);
    return true;
  }
  if (same(name, "marauder")) {
    _ctx.handleMenuAction(MENU_ACTION_OPEN_MARAUDER);
    return true;
  }
  return false;
}

void SerialTerminal::printScanItems(const char *title, const ScanItem *items, int count, bool isWifi) const {
  Serial.printf("%s list count=%d\n", title, count);
  for (int i = 0; i < count; i++) {
    if (isWifi) {
      Serial.printf("  %02d) ssid=\"%s\" bssid=%s rssi=%d ch=%d secure=%d\n", i + 1, items[i].name.c_str(), items[i].id.c_str(),
                    items[i].rssi, items[i].channel, items[i].secure ? 1 : 0);
    } else {
      Serial.printf("  %02d) name=\"%s\" addr=%s rssi=%d connectable=%d tx=%d\n", i + 1, items[i].name.c_str(), items[i].id.c_str(),
                    items[i].rssi, items[i].connectable ? 1 : 0, items[i].hasTxPower ? items[i].txPower : 0);
    }
  }
}

void SerialTerminal::printHidList(const String &categoryArg) {
  if (!_ctx.hidActions) {
    printErr("hid unavailable");
    return;
  }
  if (categoryArg.length() == 0) {
    Serial.printf("hid categories=%u\n", static_cast<unsigned>(_ctx.hidActions->categoryCount()));
    for (uint8_t i = 0; i < _ctx.hidActions->categoryCount(); i++) {
      Serial.printf("  %u) %s\n", static_cast<unsigned>(i + 1), _ctx.hidActions->categoryName(i).c_str());
    }
    return;
  }

  bool selected = false;
  uint8_t index = 0;
  if (parseU8(categoryArg, index) && index > 0) {
    selected = _ctx.hidActions->selectCategoryByIndex(index - 1);
  } else {
    selected = _ctx.hidActions->selectCategoryByName(categoryArg);
  }
  if (!selected) {
    printErr("hid category not found");
    return;
  }

  Serial.printf("hid category=\"%s\" payloads=%u\n", _ctx.hidActions->selectedCategoryName().c_str(),
                static_cast<unsigned>(_ctx.hidActions->payloadCountInSelectedCategory()));
  for (uint8_t i = 0; i < _ctx.hidActions->payloadCountInSelectedCategory(); i++) {
    const HidPayloadDef *payload = _ctx.hidActions->payloadForVisibleIndex(i);
    if (!payload) {
      continue;
    }
    Serial.printf("  %u) %s [platform=%s runtime=%us]\n", static_cast<unsigned>(i + 1), payload->title,
                  payload->platform ? payload->platform : "unknown", payload->runtimeSec);
  }
}

void SerialTerminal::printChannelSnapshot() {
  if (!_ctx.channelCopyCounts) {
    printErr("channel hooks unavailable");
    return;
  }
  int counts[15] = {0};
  _ctx.channelCopyCounts(counts, 15);
  for (int ch = 1; ch <= 14; ch++) {
    Serial.printf("ch%02d=%d%s", ch, counts[ch], (ch == 14) ? "\n" : " ");
  }
}

void SerialTerminal::printUnknown(const String &cmd) {
  Serial.printf("error: unknown command '%s' (try help)\n", cmd.c_str());
}

void SerialTerminal::printOk(const String &msg) const {
  Serial.printf("ok: %s\n", msg.c_str());
}

void SerialTerminal::printErr(const String &msg) const {
  Serial.printf("error: %s\n", msg.c_str());
}
