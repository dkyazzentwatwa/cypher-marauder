#pragma once

#include <Arduino.h>
#include <FS.h>

struct LogFileInfo {
  String name;
  size_t size = 0;
  String sessionTag;
};

class SessionLogger {
public:
  static SessionLogger &instance();

  bool begin();
  bool startSession();
  void flush();

  bool appendWifi(int rssi, int channel, bool secure, const String &ssid, const String &bssid);
  bool appendBle(int rssi, bool connectable, const String &name, const String &address, int txPower,
                 bool hasTxPower);

  String sessionId() const;
  String latestWifiFileName() const;
  String latestBleFileName() const;
  int listLogFiles(LogFileInfo *out, int maxCount) const;
  bool makeDownloadPath(const String &fileName, String &outPath) const;
  bool isMounted() const;

private:
  SessionLogger() = default;

  bool ensureLogsDir();
  bool openAppendHandles();
  void closeAppendHandles();
  bool loadBootCounter(uint32_t &counter) const;
  bool saveBootCounter(uint32_t counter) const;
  String csvEscape(const String &value) const;
  String extractSessionTag(const String &fileName) const;
  bool isValidFileName(const String &fileName) const;
  bool appendLine(const String &path, const String &line);
  bool appendToHandle(File &file, const String &line);
  void prunePrefix(const char *prefix) const;

  String _sessionId;
  String _wifiPath;
  String _blePath;
  String _latestWifiFile;
  String _latestBleFile;
  File _wifiFile;
  File _bleFile;
  uint32_t _lastFlushMs = 0;
  bool _mounted = false;
  static constexpr uint32_t MAX_SESSIONS_PER_TYPE = 30;
  static constexpr uint32_t FLUSH_INTERVAL_MS = 1000;
};
