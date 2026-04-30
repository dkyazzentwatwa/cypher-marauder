#include "WebMode.h"

#include "../include/BoardConfig.h"
#include "SessionLogger.h"

#include <LittleFS.h>

namespace {
const char *boolJson(bool value) {
  return value ? "true" : "false";
}

static void appendJsonStr(String &out, const String &val) {
  out += '"';
  for (size_t i = 0; i < val.length(); i++) {
    const char c = val[i];
    switch (c) {
    case '"':  out += "\\\""; break;
    case '\\': out += "\\\\"; break;
    case '\n': out += "\\n";  break;
    case '\r': out += "\\r";  break;
    case '\t': out += "\\t";  break;
    default:   out += c;      break;
    }
  }
  out += '"';
}

struct FsEntry {
  String path;
  size_t size = 0;
};

String escapeHtml(const String &in) {
  String out;
  out.reserve(in.length() + 8);
  for (size_t i = 0; i < in.length(); i++) {
    const char c = in[i];
    switch (c) {
    case '&': out += "&amp;"; break;
    case '<': out += "&lt;"; break;
    case '>': out += "&gt;"; break;
    case '"': out += "&quot;"; break;
    case '\'': out += "&#39;"; break;
    default: out += c; break;
    }
  }
  return out;
}

String fileBaseName(const String &path) {
  const int slash = path.lastIndexOf('/');
  if (slash < 0 || slash >= static_cast<int>(path.length()) - 1) {
    return path;
  }
  return path.substring(slash + 1);
}

bool isSafeLittleFsPath(const String &path) {
  if (path.length() == 0 || path[0] != '/') {
    return false;
  }
  if (path.indexOf("..") >= 0) {
    return false;
  }
  for (size_t i = 0; i < path.length(); i++) {
    const char c = path[i];
    const bool allowed =
        (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
        c == '/' || c == '.' || c == '_' || c == '-' || c == ' ';
    if (!allowed) {
      return false;
    }
  }
  return true;
}

int listLittleFsFiles(const String &dirPath, FsEntry *out, int maxCount) {
  if (out == nullptr || maxCount <= 0) {
    return 0;
  }
  File dir = LittleFS.open(dirPath);
  if (!dir || !dir.isDirectory()) {
    return 0;
  }
  int count = 0;
  File entry = dir.openNextFile();
  while (entry && count < maxCount) {
    if (entry.isDirectory()) {
      String nextDir = entry.name();
      if (nextDir.length() > 0 && nextDir[0] != '/') {
        nextDir = "/" + nextDir;
      }
      count += listLittleFsFiles(nextDir, out + count, maxCount - count);
    } else {
      out[count].path = entry.name();
      out[count].size = entry.size();
      count++;
    }
    entry = dir.openNextFile();
  }
  return count;
}

void appendHeader(String &html, const char *title, const char *subtitle) {
  html += "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>";
  html += title;
  html += "</title><style>body{font-family:system-ui,-apple-system,Segoe UI,Arial,sans-serif;margin:1.25rem;line-height:1.45}h1,h2{margin:.2rem 0 .75rem}section{margin:1rem 0 1.25rem;padding:1rem;border:1px solid #ddd;border-radius:.75rem}table{width:100%;border-collapse:collapse}th,td{text-align:left;padding:.45rem .4rem;border-bottom:1px solid #eee;vertical-align:top}code{background:#f5f5f5;padding:.1rem .35rem;border-radius:.25rem}a{color:#0b57d0;text-decoration:none}a:hover{text-decoration:underline}.actions{display:flex;flex-wrap:wrap;gap:.5rem}.actions form{margin:0}.actions button{padding:.55rem .8rem;border:1px solid #bbb;border-radius:.5rem;background:#fafafa;font:inherit;cursor:pointer}.pill{display:inline-block;padding:.15rem .45rem;border-radius:999px;background:#f2f2f2;margin-right:.35rem}</style></head><body>";
  html += "<h1>";
  html += title;
  html += "</h1>";
  if (subtitle != nullptr && subtitle[0] != '\0') {
    html += "<p>";
    html += subtitle;
    html += "</p>";
  }
}

void appendFooter(String &html) {
  html += "</body></html>";
}

void appendNav(String &html) {
  html += "<p><a href='/'>Home</a> | <a href='/files'>Filesystem</a> | <a href='/logs'>Logs</a> | <a href='/api/status'>Status JSON</a> | <a href='/json'>System JSON</a></p>";
}

void appendActionButton(String &html, const char *label, const char *action) {
  html += "<form method='post' action='";
  html += action;
  html += "'><button type='submit'>";
  html += label;
  html += "</button></form>";
}

void appendFileTable(String &html, const FsEntry *files, int count, const char *downloadRoute) {
  html += "<table><thead><tr><th>File</th><th>Size</th><th>Download</th></tr></thead><tbody>";
  if (count <= 0) {
    html += "<tr><td colspan='3'>No files found</td></tr>";
  } else {
    for (int i = 0; i < count; i++) {
      const String name = fileBaseName(files[i].path);
      html += "<tr><td><code>";
      html += escapeHtml(files[i].path);
      html += "</code></td><td>";
      html += files[i].size;
      html += "</td><td><a href='";
      html += downloadRoute;
      html += "?path=";
      html += files[i].path;
      html += "'>download ";
      html += escapeHtml(name);
      html += "</a></td></tr>";
    }
  }
  html += "</tbody></table>";
}
}

void WebMode::attachScanners(WifiTools *wifiTools, BluetoothTools *bluetoothTools) {
  _wifiTools = wifiTools;
  _bluetoothTools = bluetoothTools;
}

void WebMode::begin() {
  if (_active) {
    return;
  }

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  SessionLogger &logger = SessionLogger::instance();
  if (!logger.isMounted()) {
    logger.begin();
  }
  if (logger.sessionId().length() == 0) {
    logger.startSession();
  }
  configureRoutes();
  _server.begin();
  _active = true;
  _painted = false;
  _statusJsonCache = "";
  _lastStatusBuildMs = 0;
  refreshStatusCache(true);
  Serial.printf("AP started: %s password=%s ip=%s\n", AP_SSID, AP_PASSWORD,
                WiFi.softAPIP().toString().c_str());
}

void WebMode::poll() {
  if (!_active) {
    return;
  }
  _server.handleClient();
  refreshStatusCache(false);
}

void WebMode::update(DisplayPort &display) {
  if (!_active) {
    return;
  }

  if (!_painted) {
    String body = "SSID: ";
    body += AP_SSID;
    body += "\nPass: ";
    body += AP_PASSWORD;
    body += "\nIP: ";
    body += WiFi.softAPIP().toString();
    body += "\nOpen / in browser";
    display.drawMessage("AP Web Server", body, "Click/5s back");
    _painted = true;
  }
}

void WebMode::stop() {
  if (!_active) {
    return;
  }
  stopAllScans();
  _server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  _active = false;
  _painted = false;
  _statusJsonCache = "";
  _lastStatusBuildMs = 0;
  Serial.println("AP stopped");
}

bool WebMode::active() const {
  return _active;
}

WebMode::ActiveRadio WebMode::activeRadio() const {
  return _activeRadio;
}

bool WebMode::requireJsonPost() {
  if (_server.method() != HTTP_POST) {
    _server.send(405, "application/json", "{\"ok\":false,\"error\":\"method_not_allowed\"}");
    return false;
  }
  return true;
}

String WebMode::jsonEscape(const String &in) const {
  String out;
  out.reserve(in.length() + 8);
  for (size_t i = 0; i < in.length(); i++) {
    const char c = in[i];
    switch (c) {
    case '"':
      out += "\\\"";
      break;
    case '\\':
      out += "\\\\";
      break;
    case '\n':
      out += "\\n";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\t':
      out += "\\t";
      break;
    default:
      out += c;
      break;
    }
  }
  return out;
}

const char *WebMode::activeRadioString() const {
  ActiveRadio effective = _activeRadio;
  if (_wifiTools != nullptr && (_wifiTools->isContinuous() || _wifiTools->isScanning())) {
    effective = RADIO_WIFI;
  } else if (_bluetoothTools != nullptr && (_bluetoothTools->isContinuous() || _bluetoothTools->isScanning())) {
    effective = RADIO_BLE;
  } else {
    effective = RADIO_NONE;
  }

  switch (effective) {
  case RADIO_WIFI:    return "wifi";
  case RADIO_BLE:     return "ble";
  default:            return "none";
  }
}

String WebMode::buildStatusJson() const {
  String json;
  json.reserve(3584);

  const char *activeRadio = activeRadioString();

  json += "{\"ok\":true,\"active_radio\":\"";
  json += activeRadio;
  json += "\",\"session\":";
  appendJsonStr(json, SessionLogger::instance().sessionId());
  json += ",\"uptime_ms\":";
  json += millis();
  json += ",\"heap\":";
  json += ESP.getFreeHeap();
  json += ",\"psram\":";
  json += ESP.getPsramSize();
  json += ",\"latest_wifi_file\":";
  appendJsonStr(json, SessionLogger::instance().latestWifiFileName());
  json += ",\"latest_ble_file\":";
  appendJsonStr(json, SessionLogger::instance().latestBleFileName());

  json += ",\"wifi\":{";
  ScanItem items[16];
  if (_wifiTools != nullptr) {
    json += "\"available\":true,\"scanning\":";
    json += boolJson(_wifiTools->isScanning());
    json += ",\"continuous\":";
    json += boolJson(_wifiTools->isContinuous());
    json += ",\"status\":";
    appendJsonStr(json, _wifiTools->statusText());
    json += ",\"count\":";
    json += _wifiTools->itemCount();
    const int n = _wifiTools->copyItems(items, 16);
    json += ",\"results\":[";
    for (int i = 0; i < n; i++) {
      if (i > 0) json += ",";
      json += "{\"name\":";
      appendJsonStr(json, items[i].name);
      json += ",\"id\":";
      appendJsonStr(json, items[i].id);
      json += ",\"rssi\":";
      json += items[i].rssi;
      json += ",\"channel\":";
      json += items[i].channel;
      json += ",\"secure\":";
      json += boolJson(items[i].secure);
      json += "}";
    }
    json += "]";
  } else {
    json += "\"available\":false,\"scanning\":false,\"continuous\":false,\"status\":\"Unavailable\",\"count\":0,\"results\":[]";
  }
  json += "},";

  json += "\"ble\":{";
  if (_bluetoothTools != nullptr) {
    json += "\"available\":";
    json += boolJson(_bluetoothTools->available());
    json += ",\"scanning\":";
    json += boolJson(_bluetoothTools->isScanning());
    json += ",\"continuous\":";
    json += boolJson(_bluetoothTools->isContinuous());
    json += ",\"status\":";
    appendJsonStr(json, _bluetoothTools->statusText());
    json += ",\"count\":";
    json += _bluetoothTools->itemCount();
    const int n = _bluetoothTools->copyItems(items, 16);
    json += ",\"results\":[";
    for (int i = 0; i < n; i++) {
      if (i > 0) json += ",";
      json += "{\"name\":";
      appendJsonStr(json, items[i].name);
      json += ",\"id\":";
      appendJsonStr(json, items[i].id);
      json += ",\"rssi\":";
      json += items[i].rssi;
      json += ",\"connectable\":";
      json += boolJson(items[i].connectable);
      json += ",\"has_tx\":";
      json += boolJson(items[i].hasTxPower);
      json += ",\"tx_power\":";
      json += items[i].txPower;
      json += "}";
    }
    json += "]";
  } else {
    json += "\"available\":false,\"scanning\":false,\"continuous\":false,\"status\":\"Unavailable\",\"count\":0,\"results\":[]";
  }
  json += "}}";

  return json;
}

void WebMode::refreshStatusCache(bool force) {
  if (!_active) {
    return;
  }
  const uint32_t now = millis();
  if (!force && _statusJsonCache.length() > 0 && (now - _lastStatusBuildMs) < STATUS_CACHE_INTERVAL_MS) {
    return;
  }
  _statusJsonCache = buildStatusJson();
  _lastStatusBuildMs = now;
}

void WebMode::stopAllScans() {
  if (_wifiTools != nullptr) {
    _wifiTools->stop();
  }
  if (_bluetoothTools != nullptr) {
    _bluetoothTools->stop();
  }
  _activeRadio = RADIO_NONE;
}

bool WebMode::startWifiContinuous(String &message, int &httpCode) {
  if (_wifiTools == nullptr || _bluetoothTools == nullptr) {
    message = "scanner_unavailable";
    httpCode = 500;
    return false;
  }
  if (_activeRadio == RADIO_WIFI && _wifiTools->isContinuous()) {
    message = "wifi_already_running";
    httpCode = 409;
    return false;
  }
  if (_activeRadio == RADIO_BLE) {
    _bluetoothTools->stop();
  }
  _wifiTools->startContinuous();
  _activeRadio = RADIO_WIFI;
  message = "wifi_started";
  httpCode = 200;
  return true;
}

bool WebMode::stopWifiContinuous(String &message, int &httpCode) {
  if (_wifiTools == nullptr) {
    message = "wifi_unavailable";
    httpCode = 500;
    return false;
  }
  _wifiTools->stop();
  if (_activeRadio == RADIO_WIFI) {
    _activeRadio = RADIO_NONE;
  }
  message = "wifi_stopped";
  httpCode = 200;
  return true;
}

bool WebMode::startBleContinuous(String &message, int &httpCode) {
  if (_wifiTools == nullptr || _bluetoothTools == nullptr) {
    message = "scanner_unavailable";
    httpCode = 500;
    return false;
  }
  if (!_bluetoothTools->available()) {
    message = "ble_unavailable";
    httpCode = 409;
    return false;
  }
  if (_activeRadio == RADIO_BLE && _bluetoothTools->isContinuous()) {
    message = "ble_already_running";
    httpCode = 409;
    return false;
  }
  if (_activeRadio == RADIO_WIFI) {
    _wifiTools->stop();
  }
  if (!_bluetoothTools->startContinuous()) {
    message = "ble_start_failed";
    httpCode = 500;
    return false;
  }
  _activeRadio = RADIO_BLE;
  message = "ble_started";
  httpCode = 200;
  return true;
}

bool WebMode::stopBleContinuous(String &message, int &httpCode) {
  if (_bluetoothTools == nullptr) {
    message = "ble_unavailable";
    httpCode = 500;
    return false;
  }
  _bluetoothTools->stop();
  if (_activeRadio == RADIO_BLE) {
    _activeRadio = RADIO_NONE;
  }
  message = "ble_stopped";
  httpCode = 200;
  return true;
}

void WebMode::configureRoutes() {
  _server.on("/", HTTP_GET, [this]() {
    refreshStatusCache(false);
    String html;
    html.reserve(4096);
    appendHeader(html, "AP Console", "Plain file and log console.");
    appendNav(html);
    html += "<section><h2>Current State</h2><table>";
    html += "<tr><th>Session</th><td><code>";
    html += escapeHtml(SessionLogger::instance().sessionId());
    html += "</code></td></tr><tr><th>Active radio</th><td><code>";
    html += escapeHtml(activeRadioString());
    html += "</code></td></tr><tr><th>Latest WiFi log</th><td><code>";
    html += escapeHtml(SessionLogger::instance().latestWifiFileName());
    html += "</code></td></tr><tr><th>Latest BLE log</th><td><code>";
    html += escapeHtml(SessionLogger::instance().latestBleFileName());
    html += "</code></td></tr></table></section>";
    html += "<section><h2>Browse Data</h2><ul>";
    html += "<li><a href='/files'>Filesystem</a></li>";
    html += "<li><a href='/logs'>Logs</a></li>";
    html += "<li><a href='/api/status'>Status JSON</a></li>";
    html += "<li><a href='/json'>System JSON</a></li>";
    html += "</ul></section>";
    html += "<section><h2>Manual Controls</h2><div class='actions'>";
    appendActionButton(html, "Start WiFi", "/api/scan/wifi/start");
    appendActionButton(html, "Stop WiFi", "/api/scan/wifi/stop");
    appendActionButton(html, "Start BLE", "/api/scan/ble/start");
    appendActionButton(html, "Stop BLE", "/api/scan/ble/stop");
    appendActionButton(html, "Stop All", "/api/scan/stop");
    html += "</div></section>";
    appendFooter(html);
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "text/html", html);
  });

  _server.on("/files", HTTP_GET, [this]() {
    FsEntry files[128];
    const int count = listLittleFsFiles("/", files, 128);

    String html;
    html.reserve(2048 + count * 128);
    appendHeader(html, "Filesystem", "Browse LittleFS contents and download files.");
    appendNav(html);
    html += "<section><h2>LittleFS</h2>";
    appendFileTable(html, files, count, "/files/download");
    html += "</section>";
    appendFooter(html);
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "text/html", html);
  });

  _server.on("/files/list", HTTP_GET, [this]() {
    FsEntry files[128];
    const int count = listLittleFsFiles("/", files, 128);

    String json;
    json.reserve(128 + count * 80);
    json += "{\"count\":";
    json += count;
    json += ",\"files\":[";
    for (int i = 0; i < count; i++) {
      if (i > 0) json += ",";
      json += "{\"path\":";
      appendJsonStr(json, files[i].path);
      json += ",\"size\":";
      json += files[i].size;
      json += "}";
    }
    json += "]}";
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", json);
  });

  _server.on("/files/download", HTTP_GET, [this]() {
    if (!_server.hasArg("path")) {
      _server.send(400, "text/plain", "missing path");
      return;
    }
    const String path = _server.arg("path");
    if (!isSafeLittleFsPath(path) || !LittleFS.exists(path)) {
      _server.send(404, "text/plain", "file not found");
      return;
    }
    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
      _server.send(500, "text/plain", "open failed");
      return;
    }
    const String name = fileBaseName(path);
    _server.sendHeader("Cache-Control", "no-store");
    _server.sendHeader("Content-Disposition", "attachment; filename=\"" + name + "\"");
    _server.streamFile(file, "application/octet-stream");
    file.close();
  });

  _server.on("/api/status", HTTP_GET, [this]() {
    refreshStatusCache(false);
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", _statusJsonCache);
  });

  _server.on("/api/scan/wifi/start", HTTP_POST, [this]() {
    if (!requireJsonPost()) return;
    String msg;
    int code = 500;
    startWifiContinuous(msg, code);
    char resp[160];
    snprintf(resp, sizeof(resp), "{\"ok\":%s,\"message\":\"%s\",\"active_radio\":\"%s\"}",
             boolJson(code == 200), msg.c_str(), activeRadioString());
    _server.send(code, "application/json", resp);
  });

  _server.on("/api/scan/wifi/stop", HTTP_POST, [this]() {
    if (!requireJsonPost()) return;
    String msg;
    int code = 500;
    stopWifiContinuous(msg, code);
    char resp[160];
    snprintf(resp, sizeof(resp), "{\"ok\":%s,\"message\":\"%s\",\"active_radio\":\"%s\"}",
             boolJson(code == 200), msg.c_str(), activeRadioString());
    _server.send(code, "application/json", resp);
  });

  _server.on("/api/scan/ble/start", HTTP_POST, [this]() {
    if (!requireJsonPost()) return;
    String msg;
    int code = 500;
    startBleContinuous(msg, code);
    char resp[160];
    snprintf(resp, sizeof(resp), "{\"ok\":%s,\"message\":\"%s\",\"active_radio\":\"%s\"}",
             boolJson(code == 200), msg.c_str(), activeRadioString());
    _server.send(code, "application/json", resp);
  });

  _server.on("/api/scan/ble/stop", HTTP_POST, [this]() {
    if (!requireJsonPost()) return;
    String msg;
    int code = 500;
    stopBleContinuous(msg, code);
    char resp[160];
    snprintf(resp, sizeof(resp), "{\"ok\":%s,\"message\":\"%s\",\"active_radio\":\"%s\"}",
             boolJson(code == 200), msg.c_str(), activeRadioString());
    _server.send(code, "application/json", resp);
  });

  _server.on("/api/scan/stop", HTTP_POST, [this]() {
    if (!requireJsonPost()) {
      return;
    }
    stopAllScans();
    _server.send(200, "application/json", "{\"ok\":true,\"message\":\"all_stopped\",\"active_radio\":\"none\"}");
  });

  _server.on("/json", HTTP_GET, [this]() {
    char buf[96];
    snprintf(buf, sizeof(buf),
             "{\"chip\":\"ESP32-S3\",\"heap\":%u,\"psram\":%u,\"uptime_ms\":%lu}",
             ESP.getFreeHeap(), ESP.getPsramSize(), (unsigned long)millis());
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", buf);
  });

  _server.on("/logs", HTTP_GET, [this]() {
    LogFileInfo files[80];
    const int count = SessionLogger::instance().listLogFiles(files, 80);

    String html;
    html.reserve(2600 + count * 180);
    appendHeader(html, "Logs", "Session CSV logs stored under LittleFS.");
    appendNav(html);
    html += "<section><h2>Session</h2><p><code>";
    html += escapeHtml(SessionLogger::instance().sessionId());
    html += "</code></p></section>";
    html += "<section><h2>Log files</h2><table><thead><tr><th>File</th><th>Size</th><th>Session</th><th>Download</th></tr></thead><tbody>";
    if (count <= 0) {
      html += "<tr><td colspan='4'>No log files found</td></tr>";
    } else {
      for (int i = 0; i < count; i++) {
        html += "<tr><td><code>";
        html += escapeHtml(files[i].name);
        html += "</code></td><td>";
        html += files[i].size;
        html += "</td><td><code>";
        html += escapeHtml(files[i].sessionTag);
        html += "</code></td><td><a href='/logs/download?name=";
        html += files[i].name;
        html += "'>download</a></td></tr>";
      }
    }
    html += "</tbody></table></section>";
    html += "<section><h2>Shortcuts</h2><ul><li><a href='/logs/latest/wifi'>Latest WiFi CSV</a></li><li><a href='/logs/latest/ble'>Latest BLE CSV</a></li><li><a href='/logs/list'>JSON list</a></li></ul></section>";
    appendFooter(html);
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "text/html", html);
  });

  _server.on("/logs/list", HTTP_GET, [this]() {
    LogFileInfo files[80];
    const int count = SessionLogger::instance().listLogFiles(files, 80);

    String json;
    json.reserve(128 + count * 90);
    json += "{\"session\":\"";
    json += SessionLogger::instance().sessionId();
    json += "\",\"count\":";
    json += count;
    json += ",\"files\":[";
    for (int i = 0; i < count; i++) {
      if (i > 0) json += ",";
      json += "{\"name\":\"";
      json += files[i].name;
      json += "\",\"size\":";
      json += files[i].size;
      json += ",\"session\":\"";
      json += files[i].sessionTag;
      json += "\"}";
    }
    json += "]}";
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", json);
  });

  _server.on("/logs/download", HTTP_GET, [this]() {
    if (!_server.hasArg("name")) {
      _server.send(400, "text/plain", "missing name");
      return;
    }

    String path;
    const String name = _server.arg("name");
    if (!SessionLogger::instance().makeDownloadPath(name, path)) {
      _server.send(404, "text/plain", "file not found");
      return;
    }

    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
      _server.send(500, "text/plain", "open failed");
      return;
    }

    _server.sendHeader("Cache-Control", "no-store");
    _server.sendHeader("Content-Disposition", "attachment; filename=\"" + name + "\"");
    _server.streamFile(file, "text/csv");
    file.close();
  });

  _server.on("/logs/latest/wifi", HTTP_GET, [this]() {
    const String name = SessionLogger::instance().latestWifiFileName();
    if (name.length() == 0) {
      _server.send(404, "text/plain", "no wifi logs");
      return;
    }
    String path;
    if (!SessionLogger::instance().makeDownloadPath(name, path)) {
      _server.send(404, "text/plain", "no wifi logs");
      return;
    }
    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
      _server.send(500, "text/plain", "open failed");
      return;
    }
    _server.sendHeader("Cache-Control", "no-store");
    _server.sendHeader("Content-Disposition", "attachment; filename=\"" + name + "\"");
    _server.streamFile(file, "text/csv");
    file.close();
  });

  _server.on("/logs/latest/ble", HTTP_GET, [this]() {
    const String name = SessionLogger::instance().latestBleFileName();
    if (name.length() == 0) {
      _server.send(404, "text/plain", "no ble logs");
      return;
    }
    String path;
    if (!SessionLogger::instance().makeDownloadPath(name, path)) {
      _server.send(404, "text/plain", "no ble logs");
      return;
    }
    File file = LittleFS.open(path, FILE_READ);
    if (!file) {
      _server.send(500, "text/plain", "open failed");
      return;
    }
    _server.sendHeader("Cache-Control", "no-store");
    _server.sendHeader("Content-Disposition", "attachment; filename=\"" + name + "\"");
    _server.streamFile(file, "text/csv");
    file.close();
  });

}
