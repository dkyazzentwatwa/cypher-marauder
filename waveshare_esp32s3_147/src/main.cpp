#include <Arduino.h>
#include <Wire.h>

#include "../include/BoardConfig.h"
#include "AppMenu.h"
#include "BluetoothTools.h"
#include "ButtonHandler.h"
#include "DisplayPort.h"
#include "HidActions.h"
#include "MultiButtonInput.h"
#include "SessionLogger.h"
#include "SerialTerminal.h"
#include "SystemInfo.h"
#include "WebMode.h"
#include "WifiTools.h"
#include "CaptivePortal.h"
#include "SDCardManager.h"
#include "MarauderCore.h"

#if ENABLE_TOUCH
#include "../drivers/esp_lcd_touch_axs5106l/esp_lcd_touch_axs5106l.h"
#endif

DisplayPort display;
#if BOARD_HAS_THREE_BUTTONS
MultiButtonInput navButtons(PIN_BTN_UP, PIN_BTN_SELECT, PIN_BTN_BACK, BTN_ACTIVE_LOW);
#else
ButtonHandler bootButton(PIN_BOOT_BUTTON, BOOT_BUTTON_ACTIVE_LOW,
                          BOOT_BUTTON_AUTODETECT_POLARITY);
#endif
AppMenu menu;
WifiTools wifiTools;
BluetoothTools bluetoothTools;
WebMode webMode;
HidActions hidActions;
CaptivePortal captivePortal;
SerialTerminal serialTerminal;

#if ENABLE_TOUCH
TwoWire touchWire(0);
#endif
AppScreen currentScreen = SCREEN_MENU;
bool touchReady = false;
bool channelMonitorActive = false;
bool channelMonitorScanning = false;
bool channelMonitorPainted = false;
uint32_t channelMonitorLastScanMs = 0;
uint32_t channelMonitorLastPaintHash = 0;
int channelMonitorCounts[15] = {0};

void openScreen(AppScreen screen);
bool handlePortalButton(ButtonEvent event);
void goBackToMenu();
void runDisplayTest();
void updateTouchTest();
void updateChannelMonitor();
void updateMarauderScreen(bool force = false);
bool handleMarauderButton(ButtonEvent event);
void onEnterChannelMonitor();
void onExitChannelMonitor();
void startChannelMonitorScan();
void handleMenuAction(MenuAction action);
void updateTouchNavigation();
AppScreen serialCurrentScreen();
void channelStartFromSerial();
void channelStopFromSerial();
bool channelIsActiveForSerial();
bool channelIsScanningForSerial();
uint32_t channelLastScanAgeSecForSerial();
void channelCopyCountsForSerial(int *outCounts, int maxCount);

static void drawIntroScreen(DisplayPort &display) {
  Adafruit_GFX &gfx = display.gfx();
  constexpr uint16_t kIntroPhaseDelayMs = 2500;
  const uint16_t gradientA[] = {0x0000, 0x0028, 0x0070, 0x00D8, 0x01DF};
  const uint16_t gradientB[] = {0x0000, 0x1005, 0x280C, 0x400F, 0x6812};

  for (uint8_t phase = 0; phase < 2; phase++) {
    display.clear(ST77XX_BLACK);

    for (int y = 0; y < display.height(); y += 3) {
      const int blend = (y * 4) / max(1, static_cast<int>(display.height() - 1));
      const uint16_t color = (phase == 0) ? gradientA[blend] : gradientB[blend];
      gfx.fillRect(0, y, display.width(), 3, color);
    }

    for (uint8_t i = 0; i < 5; i++) {
      const int y = 30 + i * 40 + (phase ? 8 : 0);
      const uint16_t lineColor = (phase == 0) ? 0x2A7F : 0x5220;
      gfx.drawFastHLine(0, y, display.width(), lineColor);
      gfx.drawFastHLine(0, y + 1, display.width(), lineColor);
    }

    const int haloX = phase ? 138 : 34;
    const int haloY = phase ? 58 : 194;
    for (int r = 30; r >= 12; r -= 6) {
      const uint16_t haloColor = (phase == 0) ? 0x03EF : 0xFBE0;
      gfx.drawCircle(haloX, haloY, r, haloColor);
    }

    gfx.setTextSize(1);
    gfx.setTextColor(0xD71C, ST77XX_BLACK);
    gfx.setCursor(34, 14);
    gfx.print("CYPHER-DRIVE");

    gfx.fillRoundRect(14, 118, 144, 74, 10, ST77XX_BLACK);
    gfx.drawRoundRect(14, 118, 144, 74, 10, (phase == 0) ? 0x03EF : 0xFBE0);
    gfx.drawRoundRect(16, 120, 140, 70, 8, 0x31A6);
    gfx.setTextSize(2);
    gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    gfx.setCursor(24, 134);
    gfx.print("cypher-drive");
    gfx.setTextSize(1);
    if (phase == 0) {
      gfx.setTextColor(0x9E3F, ST77XX_BLACK);
      gfx.setCursor(31, 162);
      gfx.print("Initializing radios");
      gfx.setCursor(40, 176);
      gfx.print("Preparing UI...");
    } else {
      gfx.setTextColor(0x87F0, ST77XX_BLACK);
      gfx.setCursor(29, 162);
      gfx.print("[ WiFi . BLE . HID ]");
      gfx.setCursor(56, 176);
      gfx.print("Ready");
    }

    delay(kIntroPhaseDelayMs);
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(200);
  Serial.println();
  Serial.printf("cypher-drive profile=%s\n", BOARD_PROFILE_NAME);

  if (!SessionLogger::instance().begin() || !SessionLogger::instance().startSession()) {
    Serial.println("Logger init failed");
  } else {
    Serial.printf("Session: %s\n", SessionLogger::instance().sessionId().c_str());
  }

  display.begin();
  if (display.available()) {
    drawIntroScreen(display);
  }

  if (SDCardManager::instance().begin()) {
    Serial.println("SD card ready");
  } else {
    Serial.printf("SD card unavailable: %s\n", SDCardManager::instance().getErrorMessage());
  }

#if BOARD_HAS_THREE_BUTTONS
  navButtons.begin();
#else
  bootButton.begin();
#endif
  menu.begin(&display);
  wifiTools.begin();
  bluetoothTools.begin();
  hidActions.begin();
  webMode.attachScanners(&wifiTools, &bluetoothTools);

  SerialTerminalContext terminalCtx;
  terminalCtx.wifiTools = &wifiTools;
  terminalCtx.bluetoothTools = &bluetoothTools;
  terminalCtx.webMode = &webMode;
  terminalCtx.hidActions = &hidActions;
  terminalCtx.captivePortal = &captivePortal;
  terminalCtx.getCurrentScreen = serialCurrentScreen;
  terminalCtx.openScreen = openScreen;
  terminalCtx.goBackToMenu = goBackToMenu;
  terminalCtx.handleMenuAction = handleMenuAction;
  terminalCtx.channelStart = channelStartFromSerial;
  terminalCtx.channelStop = channelStopFromSerial;
  terminalCtx.channelIsActive = channelIsActiveForSerial;
  terminalCtx.channelIsScanning = channelIsScanningForSerial;
  terminalCtx.channelLastScanAgeSec = channelLastScanAgeSecForSerial;
  terminalCtx.channelCopyCounts = channelCopyCountsForSerial;
  serialTerminal.begin(terminalCtx);

#if ENABLE_TOUCH
  touchWire.begin(PIN_TOUCH_SDA, PIN_TOUCH_SCL);
  bsp_touch_init(&touchWire, PIN_TOUCH_RST, PIN_TOUCH_INT, LCD_ROTATION, LCD_WIDTH, LCD_HEIGHT);
  touchReady = true;
  Serial.println("Touch init requested");
#endif

  if (display.available()) {
    menu.draw(currentScreen, true);
  }
}

void loop() {
  serialTerminal.poll();
  ButtonEvent event = BUTTON_NONE;
#if BOARD_HAS_THREE_BUTTONS
  event = navButtons.update();
#else
  event = bootButton.update();
#endif
  if (event != BUTTON_NONE) {
    Serial.printf("Button event: %d\n", event);
  }
  webMode.poll();

  static unsigned long lastFrameTime = 0;
  const unsigned long currentTime = millis();
  const bool frameDue = (currentTime - lastFrameTime) >= 33;
  if (frameDue) {
    lastFrameTime = currentTime;
  }

  bool scannerRequestedBack = false;
  if (currentScreen == SCREEN_WIFI_SCAN) {
    scannerRequestedBack = wifiTools.handleButton(event);
  } else if (currentScreen == SCREEN_BLE_SCAN) {
    scannerRequestedBack = bluetoothTools.handleButton(event);
  } else if (currentScreen == SCREEN_USB_HID) {
    scannerRequestedBack = hidActions.handleButton(event);
  }

  // Handle captive portal navigation
  bool portalRequestsBack = false;
  if (currentScreen == SCREEN_PORTAL) {
    portalRequestsBack = handlePortalButton(event);
  }
  bool marauderRequestsBack = false;
  if (currentScreen == SCREEN_MARAUDER) {
    marauderRequestsBack = handleMarauderButton(event);
  }

  if (scannerRequestedBack || portalRequestsBack || marauderRequestsBack) {
    goBackToMenu();
  } else if (currentScreen == SCREEN_WIFI_SCAN || currentScreen == SCREEN_BLE_SCAN ||
             currentScreen == SCREEN_USB_HID || currentScreen == SCREEN_MARAUDER) {
    // Scanner screens handle click/hold interactions internally.
  } else {
    const MenuAction action = menu.handleButton(event, currentScreen);
    handleMenuAction(action);
  }
  updateTouchNavigation();

  wifiTools.update(display, currentScreen == SCREEN_WIFI_SCAN);
  bluetoothTools.update(display, currentScreen == SCREEN_BLE_SCAN);
  hidActions.update(display, currentScreen == SCREEN_USB_HID);

  if (!frameDue) {
    return;
  }

  switch (currentScreen) {
  case SCREEN_MENU:
    menu.draw(currentScreen);
    break;
  case SCREEN_AP_SERVER:
    webMode.update(display);
    break;
  case SCREEN_PORTAL:
    captivePortal.update(display);
    break;
  case SCREEN_MARAUDER:
    updateMarauderScreen();
    break;
  case SCREEN_CHANNEL_MONITOR:
    updateChannelMonitor();
    break;
  case SCREEN_TOUCH_TEST:
    updateTouchTest();
    break;
  default:
    break;
  }
}

AppScreen serialCurrentScreen() {
  return currentScreen;
}

void channelStartFromSerial() {
  handleMenuAction(MENU_ACTION_OPEN_CHANNEL);
}

void channelStopFromSerial() {
  if (currentScreen == SCREEN_CHANNEL_MONITOR) {
    goBackToMenu();
  } else {
    onExitChannelMonitor();
  }
}

bool channelIsActiveForSerial() {
  return channelMonitorActive;
}

bool channelIsScanningForSerial() {
  return channelMonitorScanning;
}

uint32_t channelLastScanAgeSecForSerial() {
  const uint32_t now = millis();
  if (channelMonitorScanning) {
    return 0;
  }
  return (now - channelMonitorLastScanMs) / 1000UL;
}

void channelCopyCountsForSerial(int *outCounts, int maxCount) {
  if (outCounts == nullptr || maxCount <= 0) {
    return;
  }
  const int copyCount = min(maxCount, 15);
  for (int i = 0; i < copyCount; i++) {
    outCounts[i] = channelMonitorCounts[i];
  }
}

void handleMenuAction(MenuAction action) {
  switch (action) {
  case MENU_ACTION_OPEN_WIFI:
    bluetoothTools.stop();
    webMode.stop();
    openScreen(SCREEN_WIFI_SCAN);
    wifiTools.startScan();
    break;
  case MENU_ACTION_OPEN_BLE:
    wifiTools.stop();
    webMode.stop();
    openScreen(SCREEN_BLE_SCAN);
    bluetoothTools.startScan();
    break;
  case MENU_ACTION_OPEN_AP:
    openScreen(SCREEN_AP_SERVER);
    webMode.begin();
    break;
  case MENU_ACTION_OPEN_SYSTEM:
    openScreen(SCREEN_SYSTEM_INFO);
    drawSystemInfo(display);
    break;
  case MENU_ACTION_OPEN_USB_HID:
    bluetoothTools.stop();
    wifiTools.stop();
    webMode.stop();
    openScreen(SCREEN_USB_HID);
    break;
  case MENU_ACTION_OPEN_CHANNEL:
    bluetoothTools.stop();
    wifiTools.stop();
    webMode.stop();
    openScreen(SCREEN_CHANNEL_MONITOR);
    break;
  case MENU_ACTION_OPEN_DISPLAY_TEST:
    openScreen(SCREEN_DISPLAY_TEST);
    runDisplayTest();
    break;
case MENU_ACTION_OPEN_TOUCH_TEST:
    openScreen(SCREEN_TOUCH_TEST);
    display.drawMessage("Touch Test", touchReady ? "Touch the screen." : "Touch disabled.", "Click/5s back");
    break;
case MENU_ACTION_OPEN_PORTAL:
    bluetoothTools.stop();
    wifiTools.stop();
    webMode.stop();
    hidActions.stop();
    captivePortal.stop();  // Reset to menu state
    captivePortal.begin(); // Start in menu state (not active)
    openScreen(SCREEN_PORTAL);
    break;
case MENU_ACTION_OPEN_MARAUDER:
    bluetoothTools.stop();
    wifiTools.stop();
    webMode.stop();
    hidActions.stop();
    openScreen(SCREEN_MARAUDER);
    updateMarauderScreen(true);
    break;
case MENU_ACTION_OPEN_ABOUT:
    openScreen(SCREEN_ABOUT);
    drawAbout(display);
    break;
  case MENU_ACTION_BACK:
    goBackToMenu();
    break;
  case MENU_ACTION_NONE:
    break;
  }
}

bool handleMarauderButton(ButtonEvent event) {
  if (event == BUTTON_NONE) return false;
  if (event == BUTTON_HOLD_5S || event == BUTTON_BACK) {
    MarauderCore::core().stopAll();
    return true;
  }
  if (event == BUTTON_CLICK) {
    MarauderCore::core().menuNext();
    updateMarauderScreen(true);
  } else if (event == BUTTON_PREV) {
    MarauderCore::core().menuPrev();
    updateMarauderScreen(true);
  } else if (event == BUTTON_SELECT || event == BUTTON_HOLD_2S) {
    MarauderCore::core().menuSelect();
    updateMarauderScreen(true);
  }
  return false;
}

void openScreen(AppScreen screen) {
  if (screen == currentScreen) {
    return;
  }
  if (currentScreen == SCREEN_CHANNEL_MONITOR) {
    onExitChannelMonitor();
  }
  currentScreen = screen;
  if (currentScreen == SCREEN_CHANNEL_MONITOR) {
    onEnterChannelMonitor();
  }
}

bool handlePortalButton(ButtonEvent event) {
  if (event == BUTTON_NONE) return false;
  
  if (event == BUTTON_HOLD_5S || event == BUTTON_BACK) {
    captivePortal.stop();
    return true;
  }
  
  if (!captivePortal.active()) {
    // In menu screen - navigate templates
    if (event == BUTTON_CLICK) {
      // Move to next template
      int idx = captivePortal.getSelectedIndex();
      idx = (idx + 1) % TEMPLATE_COUNT;
      captivePortal.setSelectedIndex(idx);
    } else if (event == BUTTON_PREV) {
      int idx = captivePortal.getSelectedIndex();
      idx = (idx - 1 + TEMPLATE_COUNT) % TEMPLATE_COUNT;
      captivePortal.setSelectedIndex(idx);
    } else if (event == BUTTON_SELECT || event == BUTTON_HOLD_2S) {
      // Launch portal with selected template
      captivePortal.selectTemplate((PortalTemplate)captivePortal.getSelectedIndex());
      captivePortal.stop();  // Reset
      captivePortal.begin();  // Start with new template
      captivePortal.setScreen(PORTAL_SCREEN_STATUS);
    }
  } else {
    // Portal running - show status or captures
    if (event == BUTTON_CLICK) {
      captivePortal.setScreen(PORTAL_SCREEN_CAPTURES);
    } else if (event == BUTTON_SELECT) {
      captivePortal.setScreen(PORTAL_SCREEN_STATUS);
    }
  }
  return false;
}

void goBackToMenu() {
  wifiTools.stop();
  bluetoothTools.stop();
  hidActions.stop();
  captivePortal.stop();
  SessionLogger::instance().flush();
  webMode.stop();
  openScreen(SCREEN_MENU);
  menu.draw(currentScreen, true);
}

void updateMarauderScreen(bool force) {
  static uint32_t lastDrawMs = 0;
  if (!force && millis() - lastDrawMs < 500) {
    return;
  }
  lastDrawMs = millis();
  String line1, line2, line3, line4;
  MarauderCore::core().renderMenuLines(line1, line2, line3, line4);
  String body = line2 + "\n" + line3 + "\n" + line4;
  display.drawMessage(line1.c_str(), body, "Click next  Hold select");
}

void runDisplayTest() {
  Adafruit_GFX &gfx = display.gfx();
  display.clear(ST77XX_BLACK);
  display.drawHeader("Display Test");
  const uint16_t colors[] = {ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE, ST77XX_YELLOW, ST77XX_MAGENTA};
  for (uint8_t i = 0; i < 5; i++) {
    gfx.fillRect(8 + i * 30, 36, 24, 44, colors[i]);
  }
  gfx.drawRect(8, 94, display.width() - 16, 70, ST77XX_WHITE);
  gfx.setCursor(15, 118);
  gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  gfx.print("GFX OK");
  display.drawFooter("Click/5s back");
}

void updateChannelMonitor() {
  const uint32_t nowMs = millis();
  const int result = WiFi.scanComplete();
  if (result == WIFI_SCAN_RUNNING) {
    if (!channelMonitorPainted) {
      display.drawMessage("Channel Monitor", "Scanning channels...", "Click/5s back");
      channelMonitorPainted = true;
    }
    return;
  }

  if (result >= 0 && channelMonitorScanning) {
    memset(channelMonitorCounts, 0, sizeof(channelMonitorCounts));
    for (int i = 0; i < result; i++) {
      const int ch = WiFi.channel(i);
      if (ch >= 1 && ch <= 14) {
        channelMonitorCounts[ch]++;
      }
    }
    WiFi.scanDelete();
    channelMonitorScanning = false;
    channelMonitorLastScanMs = nowMs;
    Serial.printf("Channel monitor scan done: %d networks\n", result);
  } else if (result < 0 && channelMonitorScanning) {
    channelMonitorScanning = false;
    channelMonitorLastScanMs = nowMs;
    Serial.printf("Channel monitor scan failed result=%d\n", result);
  }

  if (!channelMonitorScanning && nowMs - channelMonitorLastScanMs > 5000) {
    startChannelMonitorScan();
  }

  int totalAps = 0;
  int busiestChannel = 0;
  int busiestCount = 0;
  int maxCount = 1;
  for (int ch = 1; ch <= 14; ch++) {
    const int count = channelMonitorCounts[ch];
    totalAps += count;
    if (count > busiestCount) {
      busiestCount = count;
      busiestChannel = ch;
    }
    if (count > maxCount) {
      maxCount = count;
    }
  }
  const uint32_t scanAgeSec = channelMonitorScanning ? 0 : ((nowMs - channelMonitorLastScanMs) / 1000UL);

  uint32_t hash = 2166136261UL;
  for (int ch = 1; ch <= 14; ch++) {
    hash ^= static_cast<uint32_t>(channelMonitorCounts[ch] + ch * 31);
    hash *= 16777619UL;
  }
  hash ^= static_cast<uint32_t>(totalAps * 13 + busiestChannel * 17 + busiestCount * 19);
  hash *= 16777619UL;
  hash ^= static_cast<uint32_t>(scanAgeSec);
  hash *= 16777619UL;
  hash ^= channelMonitorScanning ? 0xABCDEFUL : 0;
  if (channelMonitorPainted && channelMonitorLastPaintHash == hash) {
    return;
  }
  channelMonitorLastPaintHash = hash;
  channelMonitorPainted = true;

  Adafruit_GFX &gfx = display.gfx();
  constexpr uint16_t colorBg = ST77XX_BLACK;
  constexpr uint16_t colorGuide = 0x18C3;
  constexpr uint16_t colorFrame = 0x2945;
  constexpr uint16_t colorStatus = 0x10A2;
  constexpr uint16_t colorAccent = ST77XX_YELLOW;
  constexpr uint16_t colorBar = ST77XX_CYAN;
  constexpr int bodyTop = 24;
  const int bodyHeight = display.height() - 44;
  const int graphLeft = 8;
  const int graphRight = display.width() - 8;
  const int graphTop = 56;
  const int graphBottom = 210;
  const int graphHeight = graphBottom - graphTop;
  const int graphWidth = graphRight - graphLeft;
  const int slotWidth = max(8, graphWidth / 14);
  const int barWidth = max(4, slotWidth - 4);
  const int chartStartX = graphLeft + (graphWidth - slotWidth * 14) / 2;
  const int metricsTop = 226;
  const int metricsHeight = 34;

  gfx.fillRect(0, bodyTop, display.width(), bodyHeight, colorBg);
  gfx.fillRoundRect(4, 28, display.width() - 8, 20, 4, colorStatus);
  gfx.setTextColor(ST77XX_WHITE, colorStatus);
  gfx.setCursor(10, 35);
  gfx.print(channelMonitorScanning ? "Scanning channels" : "Scan complete");
  String ageLabel = channelMonitorScanning ? "updated --" : ("updated " + String(scanAgeSec) + "s");
  int ageX = display.width() - 8 - static_cast<int>(ageLabel.length()) * 6;
  if (ageX < 86) {
    ageX = 86;
  }
  gfx.setCursor(ageX, 35);
  gfx.print(ageLabel);

  gfx.drawRect(graphLeft - 2, graphTop - 2, graphWidth + 4, graphHeight + 4, colorFrame);
  for (int i = 0; i <= 4; i++) {
    const int y = graphBottom - ((graphHeight * i) / 4);
    gfx.drawFastHLine(graphLeft, y, graphWidth, (i == 0) ? colorFrame : colorGuide);
  }
  for (int ch = 1; ch <= 14; ch++) {
    int h = (channelMonitorCounts[ch] * graphHeight) / maxCount;
    if (channelMonitorCounts[ch] > 0 && h < 2) {
      h = 2;
    }
    const int x = chartStartX + (ch - 1) * slotWidth + ((slotWidth - barWidth) / 2);
    const int y = graphBottom - h;
    const bool isBusiest = (busiestCount > 0 && ch == busiestChannel);
    gfx.fillRect(x, y, barWidth, h, isBusiest ? colorAccent : colorBar);
    if (isBusiest) {
      gfx.drawRect(x - 1, y - 1, barWidth + 2, h + 2, ST77XX_WHITE);
    }
    gfx.setCursor(x + ((barWidth >= 7) ? 2 : 1), graphBottom + 7);
    gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    gfx.print(ch % 10);
  }

  gfx.drawFastHLine(0, metricsTop - 4, display.width(), colorFrame);
  const int metricGap = 4;
  const int metricLeft = 6;
  const int metricWidth = (display.width() - metricLeft * 2 - metricGap * 2) / 3;
  for (int i = 0; i < 3; i++) {
    const int x = metricLeft + i * (metricWidth + metricGap);
    gfx.drawRoundRect(x, metricsTop, metricWidth, metricsHeight, 4, colorFrame);
  }

  gfx.setTextColor(0xBDF7, ST77XX_BLACK);
  gfx.setCursor(metricLeft + 6, metricsTop + 6);
  gfx.print("APs");
  gfx.setCursor(metricLeft + 6, metricsTop + 20);
  gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  gfx.print(totalAps);

  const int centerMetricX = metricLeft + metricWidth + metricGap;
  gfx.setTextColor(0xBDF7, ST77XX_BLACK);
  gfx.setCursor(centerMetricX + 4, metricsTop + 6);
  gfx.print("Busiest");
  gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  gfx.setCursor(centerMetricX + 4, metricsTop + 20);
  if (busiestChannel > 0) {
    gfx.print("CH ");
    gfx.print(busiestChannel);
  } else {
    gfx.print("--");
  }

  const int rightMetricX = centerMetricX + metricWidth + metricGap;
  gfx.setTextColor(0xBDF7, ST77XX_BLACK);
  gfx.setCursor(rightMetricX + 4, metricsTop + 6);
  gfx.print("Count");
  gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  gfx.setCursor(rightMetricX + 4, metricsTop + 20);
  gfx.print(busiestCount);
}

void onEnterChannelMonitor() {
  channelMonitorActive = true;
  channelMonitorScanning = false;
  channelMonitorPainted = false;
  channelMonitorLastPaintHash = 0;
  memset(channelMonitorCounts, 0, sizeof(channelMonitorCounts));
  display.clear();
  display.drawHeader("Channel Monitor");
  display.drawFooter("Click/5s back");
  startChannelMonitorScan();
}

void onExitChannelMonitor() {
  channelMonitorActive = false;
  channelMonitorScanning = false;
  WiFi.scanDelete();
}

void startChannelMonitorScan() {
  WiFi.mode(WIFI_STA);
  WiFi.scanDelete();
  const int startResult = WiFi.scanNetworks(true, true);
  if (startResult == WIFI_SCAN_FAILED) {
    channelMonitorScanning = false;
    channelMonitorLastScanMs = millis();
    Serial.println("Channel monitor scan start failed");
    return;
  }
  channelMonitorScanning = true;
  channelMonitorLastScanMs = millis();
  channelMonitorLastPaintHash = 0;
  Serial.printf("Channel monitor scan started result=%d\n", startResult);
}

void updateTouchTest() {
#if ENABLE_TOUCH
  if (!touchReady) {
    return;
  }
  bsp_touch_read();
  touch_data_t touchData;
  if (bsp_touch_get_coordinates(&touchData)) {
    Adafruit_GFX &gfx = display.gfx();
    for (uint8_t i = 0; i < touchData.touch_num; i++) {
      gfx.fillCircle(touchData.coords[i].x, touchData.coords[i].y, 4, ST77XX_GREEN);
      Serial.printf("Touch %u: %u,%u\n", i, touchData.coords[i].x, touchData.coords[i].y);
    }
  }
#endif
}

void updateTouchNavigation() {
#if ENABLE_TOUCH
  if (!touchReady || currentScreen != SCREEN_MENU) {
    return;
  }
  static uint32_t lastTapMs = 0;
  bsp_touch_read();
  touch_data_t touchData;
  if (bsp_touch_get_coordinates(&touchData) && millis() - lastTapMs > 350) {
    lastTapMs = millis();
    handleMenuAction(menu.handleButton(BUTTON_HOLD_2S, currentScreen));
  }
#endif
}
