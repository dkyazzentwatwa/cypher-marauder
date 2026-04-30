#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "DisplayPort.h"

// Captive Portal Templates
enum PortalTemplate {
  TEMPLATE_HOTEL = 0,
  TEMPLATE_COFFEE,
  TEMPLATE_CORPORATE,
  TEMPLATE_AIRPORT,
  TEMPLATE_LIBRARY,
  TEMPLATE_CONFERENCE,
  TEMPLATE_RETAIL,
  TEMPLATE_DEVICE,
  TEMPLATE_UNIVERSITY,
  TEMPLATE_MEDICAL,
  TEMPLATE_COUNT
};

// Menu screens for portal
enum PortalScreen {
  PORTAL_SCREEN_MENU,      // Template selection
  PORTAL_SCREEN_STATUS,    // Portal running status
  PORTAL_SCREEN_CAPTURES   // View captured data
};

class CaptivePortal {
public:
  void begin();
  void update(DisplayPort &display);
  void stop();
  bool active() const;
  
  // Template management
  void selectTemplate(PortalTemplate t) { _template = t; }
  PortalTemplate getTemplate() const { return _template; }
  const char* getTemplateName() const;
  const char* getTemplateSSID() const;
  
  // Menu navigation
  PortalScreen getScreen() const { return _portalScreen; }
  void setScreen(PortalScreen s) { _portalScreen = s; _dirty = true; }
  int getSelectedIndex() const { return _menuIndex; }
  void setSelectedIndex(int i) { _menuIndex = i; _dirty = true; }
  bool isDirty() const { return _dirty; }
  void clearDirty() { _dirty = false; }
  
  // Captured data access
  int getCaptureCount() const { return _captureCount; }
  const char* getCaptureData(int idx) const;
  
private:
  // Configuration
  static constexpr const char* DEFAULT_SSID = "CaptivePortal";
  
  // Network settings
  static constexpr byte HTTP_CODE = 200;
  static constexpr byte DNS_PORT = 53;
  static IPAddress APIP;
  
  // Capture storage (circular buffer)
  static constexpr int CAPTURE_MAX = 50;
  static constexpr int CAPTURE_LEN = 128;
  
  struct CaptureEntry {
    unsigned long timestamp;
    char data[CAPTURE_LEN];
  };
  
  // State
  PortalTemplate _template = TEMPLATE_HOTEL;
  PortalScreen _portalScreen = PORTAL_SCREEN_MENU;
  String currentSSID = "";
  int _menuIndex = 0;
  bool _dirty = true;
  
  // Capture storage
  CaptureEntry _captures[CAPTURE_MAX];
  int _captureCount = 0;
  int _captureIndex = 0;
  
  // Timing
  unsigned long _lastActivity = 0;
  unsigned long _lastTick = 0;
  unsigned long _tickCtr = 0;
  
  // Servers
  DNSServer dnsServer;
  WebServer webServer{80};
  
  // Display reference
  DisplayPort* _display = nullptr;
  bool _active = false;
  
  // HTML generation methods
  String input(String argName);
  String footer();
  String header(String t);
  String generateIndex();
  String generatePosted();
  String generateClear();
  
  // HTML templates (from source template)
  String getHtmlTemplate() const;
  
  // Helper methods
  void setupServers();
  void captureData(const String& data, const char* ipAddress);
  void renderMenu(DisplayPort &display);
  void renderStatus(DisplayPort &display);
  void renderCaptures(DisplayPort &display);
  void handleRoot();
  void handlePost();
  void handleClear();
};
