#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

namespace MarauderCore {

enum MonitorMode : uint8_t {
  MONITOR_OFF = 0,
  MONITOR_ALL,
  MONITOR_BEACON,
  MONITOR_PROBE,
  MONITOR_DEAUTH,
  MONITOR_EAPOL
};

struct PacketStats {
  volatile uint32_t total = 0;
  volatile uint32_t management = 0;
  volatile uint32_t control = 0;
  volatile uint32_t data = 0;
  volatile uint32_t beacon = 0;
  volatile uint32_t probe = 0;
  volatile uint32_t deauth = 0;
  volatile uint32_t disassoc = 0;
  volatile uint32_t eapol = 0;
  volatile int32_t rssiTotal = 0;
  volatile int8_t lastRssi = 0;
};

class Core {
 public:
  void begin(const char *boardName) {
    if (_begun) {
      return;
    }
    _boardName = boardName;
    _begun = true;
    Serial.println();
    Serial.printf("[marauder] core ready on %s\n", _boardName);
    Serial.println("[marauder] type 'marauder help'");
  }

  bool handleCommand(String line) {
    line.trim();
    if (line.length() == 0) {
      return false;
    }

    String lower = line;
    lower.toLowerCase();
    lower.replace("_", " ");

    if (lower == "marauder" || lower == "marauder help" || lower == "m help") {
      printHelp();
      return true;
    }
    if (lower == "marauder status" || lower == "m status") {
      printStatus();
      return true;
    }
    if (lower == "marauder wifi" || lower == "marauder ap" || lower == "m wifi" || lower == "m ap") {
      scanAccessPoints();
      return true;
    }
    if (lower == "marauder list" || lower == "m list") {
      printAccessPoints();
      return true;
    }
    if (lower.startsWith("marauder channel ") || lower.startsWith("m channel ")) {
      int channel = lastToken(lower).toInt();
      setChannel(channel);
      return true;
    }
    if (lower == "marauder hop on" || lower == "m hop on") {
      _hop = true;
      Serial.println("[marauder] channel hop on");
      return true;
    }
    if (lower == "marauder hop off" || lower == "m hop off") {
      _hop = false;
      Serial.println("[marauder] channel hop off");
      return true;
    }
    if (lower == "marauder monitor" || lower == "marauder sniff" || lower == "m monitor") {
      startMonitor(MONITOR_ALL);
      return true;
    }
    if (lower == "marauder monitor beacon" || lower == "marauder sniff beacon") {
      startMonitor(MONITOR_BEACON);
      return true;
    }
    if (lower == "marauder monitor probe" || lower == "marauder sniff probe") {
      startMonitor(MONITOR_PROBE);
      return true;
    }
    if (lower == "marauder monitor deauth" || lower == "marauder sniff deauth") {
      startMonitor(MONITOR_DEAUTH);
      return true;
    }
    if (lower == "marauder monitor eapol" || lower == "marauder sniff eapol") {
      startMonitor(MONITOR_EAPOL);
      return true;
    }
    if (lower == "marauder stop" || lower == "m stop") {
      stopMonitor();
      return true;
    }
    if (lower == "marauder reset" || lower == "m reset") {
      resetStats();
      Serial.println("[marauder] counters reset");
      return true;
    }

    return false;
  }

  void poll() {
    if (!_monitoring) {
      return;
    }

    const uint32_t now = millis();
    if (_hop && now - _lastHopMs >= _hopIntervalMs) {
      _lastHopMs = now;
      _channel++;
      if (_channel > 13) {
        _channel = 1;
      }
      esp_wifi_set_channel(_channel, WIFI_SECOND_CHAN_NONE);
    }

    if (now - _lastPrintMs >= _printIntervalMs) {
      _lastPrintMs = now;
      printStatsLine();
    }
  }

  void renderLines(String &line1, String &line2, String &line3, String &line4) const {
    line1 = _monitoring ? "Marauder: MON" : "Marauder Core";
    line2 = String("CH ") + _channel + (_hop ? " hop" : " fixed");
    line3 = String("Pkt ") + snapshotTotal() + " Dauth " + _stats.deauth;
    line4 = String("B ") + _stats.beacon + " P " + _stats.probe + " E " + _stats.eapol;
  }

  bool monitoring() const { return _monitoring; }
  uint8_t channel() const { return _channel; }
  uint32_t snapshotTotal() const { return _stats.total; }

  void startMonitor(MonitorMode mode) {
    ensureWifiPromiscReady();
    resetStats();
    _mode = mode;
    _monitoring = true;
    activeCore() = this;
    esp_wifi_set_promiscuous_rx_cb(&Core::onPacket);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(_channel, WIFI_SECOND_CHAN_NONE);
    Serial.printf("[marauder] monitor start mode=%s channel=%u hop=%s\n",
                  modeName(_mode), _channel, _hop ? "on" : "off");
  }

  void stopMonitor() {
    if (_monitoring) {
      esp_wifi_set_promiscuous(false);
    }
    _monitoring = false;
    activeCore() = nullptr;
    WiFi.mode(WIFI_STA);
    Serial.println("[marauder] monitor stopped");
  }

  void setChannel(int channel) {
    if (channel < 1 || channel > 13) {
      Serial.println("[marauder] channel must be 1-13");
      return;
    }
    _channel = static_cast<uint8_t>(channel);
    if (_monitoring) {
      esp_wifi_set_channel(_channel, WIFI_SECOND_CHAN_NONE);
    }
    Serial.printf("[marauder] channel=%u\n", _channel);
  }

  void scanAccessPoints() {
    stopMonitor();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, false);
    delay(120);

    Serial.println("[marauder] scanning APs...");
    _apCount = WiFi.scanNetworks(false, true);
    printAccessPoints();
  }

  void printAccessPoints() const {
    if (_apCount <= 0) {
      Serial.println("[marauder] no AP results. Run 'marauder wifi'.");
      return;
    }

    Serial.printf("[marauder] %d APs\n", _apCount);
    for (int i = 0; i < _apCount; i++) {
      Serial.printf("%2d ch=%2d rssi=%4d sec=%-6s bssid=%s ssid=%s\n",
                    i + 1,
                    WiFi.channel(i),
                    WiFi.RSSI(i),
                    WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "secure",
                    WiFi.BSSIDstr(i).c_str(),
                    WiFi.SSID(i).c_str());
    }
  }

  void printHelp() const {
    Serial.println();
    Serial.println("[marauder] commands");
    Serial.println("  marauder status");
    Serial.println("  marauder wifi        scan nearby APs");
    Serial.println("  marauder list        print last AP scan");
    Serial.println("  marauder monitor     promiscuous packet monitor");
    Serial.println("  marauder monitor beacon|probe|deauth|eapol");
    Serial.println("  marauder channel <1-13>");
    Serial.println("  marauder hop on|off");
    Serial.println("  marauder stop");
    Serial.println("  marauder reset");
  }

  void printStatus() const {
    Serial.println();
    Serial.printf("[marauder] board=%s heap=%u channel=%u hop=%s monitoring=%s mode=%s\n",
                  _boardName, ESP.getFreeHeap(), _channel, _hop ? "on" : "off",
                  _monitoring ? "yes" : "no", modeName(_mode));
    printStatsLine();
  }

 private:
  const char *_boardName = "unknown";
  bool _begun = false;
  bool _monitoring = false;
  bool _hop = true;
  uint8_t _channel = 1;
  int _apCount = 0;
  MonitorMode _mode = MONITOR_OFF;
  PacketStats _stats;
  uint32_t _lastHopMs = 0;
  uint32_t _lastPrintMs = 0;
  const uint32_t _hopIntervalMs = 350;
  const uint32_t _printIntervalMs = 2000;

  static Core *&activeCore() {
    static Core *ptr = nullptr;
    return ptr;
  }

  static void onPacket(void *buf, wifi_promiscuous_pkt_type_t type) {
    Core *core = activeCore();
    if (!core || !buf) {
      return;
    }
    core->processPacket(static_cast<wifi_promiscuous_pkt_t *>(buf), type);
  }

  void processPacket(const wifi_promiscuous_pkt_t *packet, wifi_promiscuous_pkt_type_t type) {
    if (!packet) {
      return;
    }

    const uint8_t *payload = packet->payload;
    const int length = packet->rx_ctrl.sig_len;
    const uint8_t frameType = (payload[0] & 0x0C) >> 2;
    const uint8_t subtype = (payload[0] & 0xF0) >> 4;
    const bool isEapol = containsEapol(payload, length);
    const bool isBeacon = frameType == 0 && subtype == 8;
    const bool isProbe = frameType == 0 && (subtype == 4 || subtype == 5);
    const bool isDeauth = frameType == 0 && subtype == 12;

    if (!modeAccepts(isBeacon, isProbe, isDeauth, isEapol)) {
      return;
    }

    _stats.total++;
    _stats.lastRssi = packet->rx_ctrl.rssi;
    _stats.rssiTotal += packet->rx_ctrl.rssi;

    if (type == WIFI_PKT_MGMT || frameType == 0) {
      _stats.management++;
    } else if (type == WIFI_PKT_CTRL || frameType == 1) {
      _stats.control++;
    } else {
      _stats.data++;
    }

    if (isBeacon) _stats.beacon++;
    if (isProbe) _stats.probe++;
    if (isDeauth) _stats.deauth++;
    if (frameType == 0 && subtype == 10) _stats.disassoc++;
    if (isEapol) _stats.eapol++;
  }

  bool modeAccepts(bool beacon, bool probe, bool deauth, bool eapol) const {
    switch (_mode) {
      case MONITOR_BEACON: return beacon;
      case MONITOR_PROBE: return probe;
      case MONITOR_DEAUTH: return deauth;
      case MONITOR_EAPOL: return eapol;
      case MONITOR_ALL: return true;
      case MONITOR_OFF:
      default: return false;
    }
  }

  static bool containsEapol(const uint8_t *payload, int length) {
    const int limit = min(length - 1, 96);
    for (int i = 0; i < limit; i++) {
      if (payload[i] == 0x88 && payload[i + 1] == 0x8e) {
        return true;
      }
    }
    return false;
  }

  void ensureWifiPromiscReady() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, false);
    esp_wifi_set_promiscuous(false);
    wifi_promiscuous_filter_t filter = {
      .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA | WIFI_PROMIS_FILTER_MASK_CTRL
    };
    esp_wifi_set_promiscuous_filter(&filter);
  }

  void resetStats() {
    _stats = PacketStats();
  }

  static String lastToken(const String &line) {
    int idx = line.lastIndexOf(' ');
    if (idx < 0) {
      return line;
    }
    return line.substring(idx + 1);
  }

  static const char *modeName(MonitorMode mode) {
    switch (mode) {
      case MONITOR_ALL: return "all";
      case MONITOR_BEACON: return "beacon";
      case MONITOR_PROBE: return "probe";
      case MONITOR_DEAUTH: return "deauth";
      case MONITOR_EAPOL: return "eapol";
      case MONITOR_OFF:
      default: return "off";
    }
  }

  void printStatsLine() const {
    const int32_t avgRssi = _stats.total ? (_stats.rssiTotal / static_cast<int32_t>(_stats.total)) : 0;
    Serial.printf("[marauder] ch=%u total=%lu mgmt=%lu data=%lu ctrl=%lu beacon=%lu probe=%lu deauth=%lu disassoc=%lu eapol=%lu rssi=%d avg=%ld\n",
                  _channel,
                  static_cast<unsigned long>(_stats.total),
                  static_cast<unsigned long>(_stats.management),
                  static_cast<unsigned long>(_stats.data),
                  static_cast<unsigned long>(_stats.control),
                  static_cast<unsigned long>(_stats.beacon),
                  static_cast<unsigned long>(_stats.probe),
                  static_cast<unsigned long>(_stats.deauth),
                  static_cast<unsigned long>(_stats.disassoc),
                  static_cast<unsigned long>(_stats.eapol),
                  _stats.lastRssi,
                  static_cast<long>(avgRssi));
  }
};

inline Core &core() {
  static Core instance;
  return instance;
}

}  // namespace MarauderCore
