#include "SessionLogger.h"

#include <LittleFS.h>

namespace {
constexpr const char *LOG_DIR = "/logs";
constexpr const char *BOOT_COUNTER_FILE = "/logs/boot_counter.txt";
}

SessionLogger &SessionLogger::instance() {
  static SessionLogger logger;
  return logger;
}

bool SessionLogger::begin() {
  if (_mounted) {
    return true;
  }
  _mounted = LittleFS.begin(false);
  if (!_mounted) {
    Serial.println("LittleFS mount failed (non-destructive mode)");
    return false;
  }
  if (!ensureLogsDir()) {
    Serial.println("LittleFS logs directory unavailable");
    return false;
  }
  return true;
}

bool SessionLogger::startSession() {
  if (!begin()) {
    return false;
  }
  closeAppendHandles();

  uint32_t bootCounter = 0;
  if (!loadBootCounter(bootCounter)) {
    bootCounter = 0;
  }
  bootCounter++;
  if (!saveBootCounter(bootCounter)) {
    Serial.println("Failed to store boot counter");
  }

  const uint32_t chipLow = static_cast<uint32_t>(ESP.getEfuseMac() & 0xFFFFFFFFULL);
  char idBuf[64];
  snprintf(idBuf, sizeof(idBuf), "boot-%08lX-%06lu-%010lu", static_cast<unsigned long>(chipLow),
           static_cast<unsigned long>(bootCounter), static_cast<unsigned long>(millis()));
  _sessionId = idBuf;

  _wifiPath = String(LOG_DIR) + "/wifi_" + _sessionId + ".csv";
  _blePath = String(LOG_DIR) + "/ble_" + _sessionId + ".csv";
  _latestWifiFile = "wifi_" + _sessionId + ".csv";
  _latestBleFile = "ble_" + _sessionId + ".csv";

  if (!LittleFS.exists(_wifiPath)) {
    File f = LittleFS.open(_wifiPath, FILE_WRITE);
    if (!f) {
      Serial.printf("Failed to create wifi log file: %s\n", _wifiPath.c_str());
      return false;
    }
    f.println("session_id,uptime_ms,type,rssi,channel,secure,ssid,bssid");
    f.close();
  }
  if (!LittleFS.exists(_blePath)) {
    File f = LittleFS.open(_blePath, FILE_WRITE);
    if (!f) {
      Serial.printf("Failed to create ble log file: %s\n", _blePath.c_str());
      return false;
    }
    f.println("session_id,uptime_ms,type,rssi,connectable,name,address,tx_power");
    f.close();
  }

  prunePrefix("wifi_");
  prunePrefix("ble_");
  if (!openAppendHandles()) {
    Serial.println("Failed to open append handles for session logger");
    return false;
  }
  _lastFlushMs = millis();
  Serial.printf("Logger session started: %s\n", _sessionId.c_str());
  return true;
}

void SessionLogger::flush() {
  if (_wifiFile) {
    _wifiFile.flush();
  }
  if (_bleFile) {
    _bleFile.flush();
  }
  _lastFlushMs = millis();
}

bool SessionLogger::appendWifi(int rssi, int channel, bool secure, const String &ssid, const String &bssid) {
  if (_sessionId.length() == 0 || _wifiPath.length() == 0) {
    return false;
  }
  String line;
  line.reserve(160);
  line += csvEscape(_sessionId);
  line += ",";
  line += String(millis());
  line += ",wifi,";
  line += String(rssi);
  line += ",";
  line += String(channel);
  line += ",";
  line += secure ? "1" : "0";
  line += ",";
  line += csvEscape(ssid);
  line += ",";
  line += csvEscape(bssid);
  return appendLine(_wifiPath, line);
}

bool SessionLogger::appendBle(int rssi, bool connectable, const String &name, const String &address, int txPower,
                              bool hasTxPower) {
  if (_sessionId.length() == 0 || _blePath.length() == 0) {
    return false;
  }
  String line;
  line.reserve(180);
  line += csvEscape(_sessionId);
  line += ",";
  line += String(millis());
  line += ",ble,";
  line += String(rssi);
  line += ",";
  line += connectable ? "1" : "0";
  line += ",";
  line += csvEscape(name);
  line += ",";
  line += csvEscape(address);
  line += ",";
  if (hasTxPower) {
    line += String(txPower);
  }
  return appendLine(_blePath, line);
}

String SessionLogger::sessionId() const {
  return _sessionId;
}

String SessionLogger::latestWifiFileName() const {
  return _latestWifiFile;
}

String SessionLogger::latestBleFileName() const {
  return _latestBleFile;
}

int SessionLogger::listLogFiles(LogFileInfo *out, int maxCount) const {
  if (out == nullptr || maxCount <= 0) {
    return 0;
  }

  int count = 0;
  File root = LittleFS.open(LOG_DIR);
  if (!root || !root.isDirectory()) {
    return 0;
  }

  File entry = root.openNextFile();
  while (entry && count < maxCount) {
    String name = entry.name();
    int slash = name.lastIndexOf('/');
    if (slash >= 0) {
      name = name.substring(slash + 1);
    }
    if ((name.startsWith("wifi_") || name.startsWith("ble_")) && name.endsWith(".csv")) {
      out[count].name = name;
      out[count].size = entry.size();
      out[count].sessionTag = extractSessionTag(name);
      count++;
    }
    entry = root.openNextFile();
  }
  return count;
}

bool SessionLogger::makeDownloadPath(const String &fileName, String &outPath) const {
  if (!isValidFileName(fileName)) {
    return false;
  }
  outPath = String(LOG_DIR) + "/" + fileName;
  return LittleFS.exists(outPath);
}

bool SessionLogger::isMounted() const {
  return _mounted;
}

bool SessionLogger::ensureLogsDir() {
  if (!LittleFS.exists(LOG_DIR)) {
    return LittleFS.mkdir(LOG_DIR);
  }
  return true;
}

bool SessionLogger::openAppendHandles() {
  closeAppendHandles();
  _wifiFile = LittleFS.open(_wifiPath, FILE_APPEND);
  if (!_wifiFile) {
    closeAppendHandles();
    return false;
  }
  _bleFile = LittleFS.open(_blePath, FILE_APPEND);
  if (!_bleFile) {
    closeAppendHandles();
    return false;
  }
  return true;
}

void SessionLogger::closeAppendHandles() {
  if (_wifiFile) {
    _wifiFile.flush();
    _wifiFile.close();
  }
  if (_bleFile) {
    _bleFile.flush();
    _bleFile.close();
  }
}

bool SessionLogger::loadBootCounter(uint32_t &counter) const {
  counter = 0;
  if (!LittleFS.exists(BOOT_COUNTER_FILE)) {
    return true;
  }
  File f = LittleFS.open(BOOT_COUNTER_FILE, FILE_READ);
  if (!f) {
    return false;
  }
  String text = f.readStringUntil('\n');
  text.trim();
  counter = static_cast<uint32_t>(strtoul(text.c_str(), nullptr, 10));
  f.close();
  return true;
}

bool SessionLogger::saveBootCounter(uint32_t counter) const {
  File f = LittleFS.open(BOOT_COUNTER_FILE, FILE_WRITE);
  if (!f) {
    return false;
  }
  f.print(counter);
  f.print('\n');
  f.close();
  return true;
}

String SessionLogger::csvEscape(const String &value) const {
  String out = "\"";
  for (size_t i = 0; i < value.length(); i++) {
    const char c = value[i];
    if (c == '"') {
      out += "\"\"";
    } else {
      out += c;
    }
  }
  out += "\"";
  return out;
}

String SessionLogger::extractSessionTag(const String &fileName) const {
  const int underscore = fileName.indexOf('_');
  const int dot = fileName.lastIndexOf('.');
  if (underscore < 0 || dot <= underscore) {
    return "";
  }
  return fileName.substring(underscore + 1, dot);
}

bool SessionLogger::isValidFileName(const String &fileName) const {
  if (fileName.length() < 8 || fileName.length() > 96) {
    return false;
  }
  if (!(fileName.startsWith("wifi_") || fileName.startsWith("ble_"))) {
    return false;
  }
  if (!fileName.endsWith(".csv")) {
    return false;
  }
  for (size_t i = 0; i < fileName.length(); i++) {
    const char c = fileName[i];
    const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
                    c == '_' || c == '-' || c == '.';
    if (!ok) {
      return false;
    }
  }
  return fileName.indexOf("..") < 0;
}

bool SessionLogger::appendLine(const String &path, const String &line) {
  if (path == _wifiPath) {
    if (!_wifiFile) {
      _wifiFile = LittleFS.open(_wifiPath, FILE_APPEND);
      if (!_wifiFile) {
        return false;
      }
    }
    return appendToHandle(_wifiFile, line);
  }
  if (path == _blePath) {
    if (!_bleFile) {
      _bleFile = LittleFS.open(_blePath, FILE_APPEND);
      if (!_bleFile) {
        return false;
      }
    }
    return appendToHandle(_bleFile, line);
  }

  File f = LittleFS.open(path, FILE_APPEND);
  if (!f) {
    return false;
  }
  const bool ok = f.println(line);
  f.close();
  return ok;
}

bool SessionLogger::appendToHandle(File &file, const String &line) {
  if (!file) {
    return false;
  }
  if (!file.println(line)) {
    return false;
  }
  const uint32_t now = millis();
  if ((now - _lastFlushMs) >= FLUSH_INTERVAL_MS) {
    flush();
  }
  return true;
}

void SessionLogger::prunePrefix(const char *prefix) const {
  String names[MAX_SESSIONS_PER_TYPE + 16];
  int count = 0;

  File root = LittleFS.open(LOG_DIR);
  if (!root || !root.isDirectory()) {
    return;
  }

  File entry = root.openNextFile();
  while (entry && count < static_cast<int>(MAX_SESSIONS_PER_TYPE + 16)) {
    String name = entry.name();
    int slash = name.lastIndexOf('/');
    if (slash >= 0) {
      name = name.substring(slash + 1);
    }
    if (name.startsWith(prefix) && name.endsWith(".csv")) {
      names[count++] = name;
    }
    entry = root.openNextFile();
  }

  for (int i = 0; i < count - 1; i++) {
    for (int j = i + 1; j < count; j++) {
      if (names[j] < names[i]) {
        String t = names[i];
        names[i] = names[j];
        names[j] = t;
      }
    }
  }

  const int removeCount = count - static_cast<int>(MAX_SESSIONS_PER_TYPE);
  for (int i = 0; i < removeCount; i++) {
    LittleFS.remove(String(LOG_DIR) + "/" + names[i]);
  }
}
