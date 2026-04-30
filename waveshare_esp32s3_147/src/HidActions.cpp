#include "HidActions.h"

#include <Adafruit_ST7789.h>
#include <ctype.h>

#include "../include/BoardConfig.h"

// ARDUINO_USB_MODE=0 means USB-OTG (TinyUSB) is selected in board settings.
// Hardware CDC mode (=1) does not support HID; the TinyUSB libraries are only
// reachable via direct #include when building in OTG mode.
#if ENABLE_HID_ACTIONS && defined(ARDUINO_USB_MODE) && ARDUINO_USB_MODE == 0
#include <USB.h>
#include <USBHIDKeyboard.h>
#if defined(KEY_LEFT_GUI) && defined(KEY_RETURN)
#define HID_USBHID_AVAILABLE 1
static USBHIDKeyboard gHidKeyboard;
#else
#define HID_USBHID_AVAILABLE 0
#endif
#else
#define HID_USBHID_AVAILABLE 0
#endif

#if ENABLE_HID_ACTIONS && !HID_USBHID_AVAILABLE
#pragma message("ENABLE_HID_ACTIONS=1 but USB headers unavailable -- set USB Mode to USB-OTG (TinyUSB) in board settings")
#endif

namespace {
constexpr uint32_t EXEC_BOOT_WAIT_MS = 3000;
constexpr uint32_t EXEC_AFTER_BEGIN_MS = 250;
constexpr uint32_t EXEC_AFTER_SPOTLIGHT_MS = 450;
constexpr uint32_t EXEC_AFTER_TERMINAL_TEXT_MS = 350;
constexpr uint32_t EXEC_AFTER_TERMINAL_OPEN_MS = 900;
constexpr uint32_t EXEC_AFTER_COMMAND_MS = 180;
constexpr uint32_t EXEC_AFTER_COMMAND_MS_IOS = 320;
constexpr uint8_t MAX_VISIBLE_ROWS = 5;
}

void HidActions::begin() {
  rebuildCategoryPayloadMap();
  enterCategoryList();
  _needsRender = true;
#if HID_USBHID_AVAILABLE
  gHidKeyboard.begin();
  USB.begin();
  _usbStarted = true;
  _available = true;
  _usbReady = (bool)USB;
  _lastStatus = _usbReady ? "Ready" : "Waiting for USB";
  Serial.printf("HID begin: available=1 usbReady=%d mode=%d cdc=%d\n", _usbReady ? 1 : 0,
#ifdef ARDUINO_USB_MODE
                ARDUINO_USB_MODE,
#else
                -1,
#endif
#ifdef ARDUINO_USB_CDC_ON_BOOT
                ARDUINO_USB_CDC_ON_BOOT
#else
                -1
#endif
  );
#else
  _available = false;
  _usbStarted = false;
  _usbReady = false;
  _lastStatus = "Need USB-OTG mode";
  Serial.println("HID begin: available=0 -- recompile with USB Mode: USB-OTG (TinyUSB)");
#endif
}

bool HidActions::handleButton(ButtonEvent event) {
  if (event == BUTTON_NONE) {
    return false;
  }

  if (_running) {
    if (event == BUTTON_HOLD_5S || event == BUTTON_BACK) {
      stopRun("Stopped");
      _uiMode = UI_MODE_PAYLOAD_LIST;
      _needsRender = true;
    }
    return false;
  }

  if (event == BUTTON_CLICK) {
    moveSelection(+1);
    _needsRender = true;
    return false;
  }
  if (event == BUTTON_PREV) {
    moveSelection(-1);
    _needsRender = true;
    return false;
  }
  if (event == BUTTON_SELECT) {
    return selectCurrent();
  }

  if (event == BUTTON_HOLD_2S) {
    moveSelection(-1);
    _needsRender = true;
    return false;
  }

  if (event == BUTTON_HOLD_5S) {
    return selectCurrent();
  }

  return false;
}

void HidActions::update(DisplayPort &display, bool drawToScreen) {
  serviceExecution();
#if HID_USBHID_AVAILABLE
  if (_usbStarted && !_running) {
    const bool ready = (bool)USB;
    if (ready != _usbReady) {
      _usbReady = ready;
      _needsRender = true;
    }
  }
#endif
  if (!drawToScreen) {
    _wasVisible = false;
    return;
  }
  if (!_wasVisible) {
    _wasVisible = true;
    _needsRender = true;
  }
  if (!_needsRender) {
    return;
  }
  draw(display);
  _needsRender = false;
}

void HidActions::setSelected(uint8_t index) {
  const uint8_t payloadCount = selectedCategoryPayloadCount();
  if (payloadCount == 0) {
    _selectedPayload = 0;
    _selectedPayloadRow = 0;
    _needsRender = true;
    return;
  }
  _selectedPayload = index % payloadCount;
  _selectedPayloadRow = _selectedPayload + 1;
  _needsRender = true;
}

uint8_t HidActions::selected() const {
  return _selectedPayload;
}

uint8_t HidActions::count() const {
  return selectedCategoryPayloadCount();
}

bool HidActions::runSelected() {
  if (!_available) {
    _lastStatus = "HID unavailable";
    _needsRender = true;
    return false;
  }

  const uint8_t payloadCount = selectedCategoryPayloadCount();
  if (_running || payloadCount == 0) {
    return false;
  }

  if (_selectedPayload >= payloadCount) {
    _selectedPayload = payloadCount - 1;
  }

  const uint16_t payloadIndex = payloadIndexForVisible(_selectedPayload);
  if (payloadIndex >= kHidPayloadCount) {
    _lastStatus = "No payload";
    _needsRender = true;
    return false;
  }

  _running = true;
  _uiMode = UI_MODE_RUNNING;
  _commandIndex = 0;
  _execState = EXEC_WAIT_USB;
  _nextActionMs = millis();
  _lastStatus = "Waiting for USB";
  _lastRunMs = millis();
  _needsRender = true;
  return true;
}

void HidActions::stop() {
  if (_running) {
    stopRun("Stopped");
  } else {
    _needsRender = true;
  }
}

bool HidActions::isRunning() const {
  return _running;
}

String HidActions::lastStatus() const {
  return _lastStatus;
}

bool HidActions::available() const {
  return _available;
}

uint8_t HidActions::categoryCount() const {
  return _categoryCount;
}

String HidActions::categoryName(uint8_t index) const {
  if (index >= _categoryCount || _categoryNames[index] == nullptr) {
    return String();
  }
  return String(_categoryNames[index]);
}

bool HidActions::selectCategoryByIndex(uint8_t index) {
  if (index >= _categoryCount) {
    return false;
  }
  enterPayloadList(index);
  _needsRender = true;
  return true;
}

bool HidActions::selectCategoryByName(const String &name) {
  String needle = name;
  needle.trim();
  needle.toLowerCase();
  for (uint8_t i = 0; i < _categoryCount; i++) {
    if (_categoryNames[i] == nullptr) {
      continue;
    }
    String candidate = _categoryNames[i];
    candidate.toLowerCase();
    if (candidate == needle) {
      enterPayloadList(i);
      _needsRender = true;
      return true;
    }
  }
  return false;
}

uint8_t HidActions::payloadCountInSelectedCategory() const {
  return selectedCategoryPayloadCount();
}

int HidActions::selectedCategoryIndex() const {
  if (_categoryCount == 0) {
    return -1;
  }
  return _selectedCategory;
}

String HidActions::selectedCategoryName() const {
  if (_categoryCount == 0 || _selectedCategory >= _categoryCount || _categoryNames[_selectedCategory] == nullptr) {
    return String();
  }
  return String(_categoryNames[_selectedCategory]);
}

bool HidActions::selectPayloadByVisibleIndex(uint8_t index) {
  const uint8_t payloadCount = selectedCategoryPayloadCount();
  if (index >= payloadCount) {
    return false;
  }
  _selectedPayload = index;
  _selectedPayloadRow = index + 1;
  _needsRender = true;
  return true;
}

const HidPayloadDef *HidActions::payloadForVisibleIndex(uint8_t index) const {
  const uint16_t payloadIndex = payloadIndexForVisible(index);
  if (payloadIndex >= kHidPayloadCount) {
    return nullptr;
  }
  return &kHidPayloads[payloadIndex];
}

const HidPayloadDef *HidActions::selectedPayloadDef() const {
  return payloadForVisibleIndex(_selectedPayload);
}

void HidActions::rebuildCategoryPayloadMap() {
  _categoryCount = 0;
  _categoryPayloadCount = 0;
  memset(_categoryNames, 0, sizeof(_categoryNames));
  memset(_categoryStarts, 0, sizeof(_categoryStarts));
  memset(_categoryCounts, 0, sizeof(_categoryCounts));

  for (uint16_t i = 0; i < kHidPayloadCount; i++) {
    const char *category = kHidPayloads[i].category;
    if (category == nullptr || category[0] == '\0') {
      category = "misc";
    }

    if (findCategoryIndex(category) != 0xFF) {
      continue;
    }

    if (_categoryCount >= MAX_CATEGORIES) {
      continue;
    }

    _categoryNames[_categoryCount++] = category;
  }

  // Alphabetical category order for stable UX.
  for (uint8_t i = 0; i < _categoryCount; i++) {
    for (uint8_t j = static_cast<uint8_t>(i + 1); j < _categoryCount; j++) {
      if (strcmp(_categoryNames[j], _categoryNames[i]) < 0) {
        const char *tmp = _categoryNames[i];
        _categoryNames[i] = _categoryNames[j];
        _categoryNames[j] = tmp;
      }
    }
  }

  for (uint16_t i = 0; i < kHidPayloadCount; i++) {
    const char *category = kHidPayloads[i].category;
    if (category == nullptr || category[0] == '\0') {
      category = "misc";
    }
    const uint8_t catIndex = findCategoryIndex(category);
    if (catIndex == 0xFF) {
      continue;
    }
    _categoryCounts[catIndex]++;
  }

  uint16_t running = 0;
  for (uint8_t i = 0; i < _categoryCount; i++) {
    _categoryStarts[i] = running;
    running += _categoryCounts[i];
  }

  _categoryPayloadCount = running;
  if (_categoryPayloadCount > MAX_CATEGORY_PAYLOAD_INDEXES) {
    _categoryPayloadCount = MAX_CATEGORY_PAYLOAD_INDEXES;
  }

  uint16_t writePos[MAX_CATEGORIES] = {0};
  for (uint8_t i = 0; i < _categoryCount; i++) {
    writePos[i] = _categoryStarts[i];
  }

  for (uint16_t i = 0; i < kHidPayloadCount; i++) {
    const char *category = kHidPayloads[i].category;
    if (category == nullptr || category[0] == '\0') {
      category = "misc";
    }
    const uint8_t catIndex = findCategoryIndex(category);
    if (catIndex == 0xFF) {
      continue;
    }

    const uint16_t slot = writePos[catIndex]++;
    if (slot < MAX_CATEGORY_PAYLOAD_INDEXES) {
      _categoryPayloadIndexes[slot] = i;
    }
  }

  if (_selectedCategory >= _categoryCount) {
    _selectedCategory = 0;
  }

  const uint8_t payloadCount = selectedCategoryPayloadCount();
  if (payloadCount == 0) {
    _selectedPayload = 0;
    _selectedPayloadRow = 0;
  } else if (_selectedPayload >= payloadCount) {
    _selectedPayload = payloadCount - 1;
    _selectedPayloadRow = _selectedPayload + 1;
  }
}

void HidActions::draw(DisplayPort &display) {
  Adafruit_GFX &gfx = display.gfx();
  display.clear(ST77XX_BLACK);
  display.drawHeader("USB HID Actions");

  const uint16_t statusColor = !_available ? ST77XX_RED : (_running ? ST77XX_GREEN : ST77XX_YELLOW);
  gfx.fillRoundRect(6, 28, display.width() - 12, 24, 6, statusColor);
  gfx.setTextColor(ST77XX_BLACK, statusColor);
  gfx.setCursor(12, 36);
  if (!_available) {
    gfx.print("HID UNAVAILABLE");
  } else if (_running) {
    gfx.print("RUNNING");
  } else {
    gfx.print("READY");
  }

  gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  gfx.setCursor(6, 56);
  gfx.print("View:");
  gfx.setCursor(48, 56);
  if (_uiMode == UI_MODE_CATEGORY_LIST) {
    gfx.print("Categories");
  } else if (_uiMode == UI_MODE_PAYLOAD_LIST) {
    gfx.print(_categoryCount > 0 ? _categoryNames[_selectedCategory] : "none");
  } else {
    gfx.print("Running");
  }

  gfx.setCursor(6, 70);
  gfx.print("Count:");
  gfx.setCursor(48, 70);
  if (_uiMode == UI_MODE_CATEGORY_LIST) {
    gfx.print(_categoryCount);
    gfx.print(" categories");
  } else {
    gfx.print(selectedCategoryPayloadCount());
    gfx.print(" payloads");
  }

  gfx.setCursor(6, 84);
  gfx.print("USB:");
  gfx.setCursor(48, 84);
  if (!_available) {
    gfx.print("n/a");
  } else if (_usbReady) {
    gfx.print("ready");
  } else if (_usbStarted) {
    gfx.print("waiting");
  } else {
    gfx.print("off");
  }

  const uint8_t payloadCount = selectedCategoryPayloadCount();
  if ((_uiMode == UI_MODE_PAYLOAD_LIST || _uiMode == UI_MODE_RUNNING) && payloadCount > 0 && _selectedPayload < payloadCount) {
    const uint16_t payloadIndex = payloadIndexForVisible(_selectedPayload);
    if (payloadIndex < kHidPayloadCount) {
      const HidPayloadDef &payloadDef = kHidPayloads[payloadIndex];
      gfx.setCursor(6, 98);
      gfx.print("Payload:");
      gfx.setCursor(58, 98);
      gfx.print(truncate(payloadDef.title, 18));
    }
  }

  const uint8_t totalRows = currentRowCount();
  const uint8_t selectedRow = currentSelectionRow();
  uint8_t firstRow = 0;

  if (selectedRow >= MAX_VISIBLE_ROWS) {
    firstRow = selectedRow - MAX_VISIBLE_ROWS + 1;
  }
  if (totalRows > MAX_VISIBLE_ROWS && firstRow + MAX_VISIBLE_ROWS > totalRows) {
    firstRow = totalRows - MAX_VISIBLE_ROWS;
  }

  for (uint8_t row = 0; row < MAX_VISIBLE_ROWS && firstRow + row < totalRows; row++) {
    const uint8_t itemRow = firstRow + row;
    const int y = 115 + row * 26;
    const bool selected = itemRow == selectedRow;
    const uint16_t bg = selected ? ST77XX_CYAN : ST77XX_BLACK;
    const uint16_t fg = selected ? ST77XX_BLACK : ST77XX_WHITE;

    gfx.drawRoundRect(6, y, display.width() - 12, 25, 5, selected ? ST77XX_CYAN : ST77XX_WHITE);
    if (selected) {
      gfx.fillRoundRect(7, y + 1, display.width() - 14, 23, 4, bg);
    }

    gfx.setTextColor(fg, bg);
    gfx.setCursor(12, y + 8);

    if (itemRow == 0) {
      if (_uiMode == UI_MODE_CATEGORY_LIST) {
        gfx.print("<- Back to menu");
      } else {
        gfx.print("<- Back to categories");
      }
    } else if (_uiMode == UI_MODE_CATEGORY_LIST) {
      const uint8_t categoryIndex = itemRow - 1;
      if (categoryIndex < _categoryCount) {
        String label = String(_categoryNames[categoryIndex]);
        label += " (";
        label += String(_categoryCounts[categoryIndex]);
        label += ")";
        gfx.print(truncate(label, 22));
      }
    } else {
      const uint8_t visibleIndex = itemRow - 1;
      const uint16_t payloadIndex = payloadIndexForVisible(visibleIndex);
      if (payloadIndex < kHidPayloadCount) {
        String label = String(visibleIndex + 1);
        label += "/";
        label += String(payloadCount);
        label += " ";
        label += truncate(kHidPayloads[payloadIndex].title, 16);
        gfx.print(label);
      }
    }
  }

  gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  gfx.setCursor(6, 248);
  gfx.print("Last:");
  gfx.setCursor(40, 248);
  gfx.print(truncate(_lastStatus, 20));
  if (_lastRunMs > 0) {
    gfx.setCursor(6, 262);
    gfx.print("At:");
    gfx.setCursor(40, 262);
    gfx.print(_lastRunMs);
    gfx.print("ms");
  }

  display.drawFooter("Click down  Hold2 up  Hold5 select");
}

void HidActions::moveSelection(int8_t delta) {
  const uint8_t rows = currentRowCount();
  if (rows == 0) {
    return;
  }

  uint8_t *selection = (_uiMode == UI_MODE_CATEGORY_LIST) ? &_selectedCategoryRow : &_selectedPayloadRow;
  if (delta > 0) {
    *selection = static_cast<uint8_t>((*selection + 1) % rows);
  } else {
    *selection = (*selection == 0) ? (rows - 1) : static_cast<uint8_t>(*selection - 1);
  }
}

bool HidActions::selectCurrent() {
  if (_uiMode == UI_MODE_CATEGORY_LIST) {
    if (_selectedCategoryRow == 0) {
      _lastStatus = "Exit HID";
      _needsRender = true;
      return true;
    }

    const uint8_t categoryIndex = _selectedCategoryRow - 1;
    if (categoryIndex < _categoryCount) {
      enterPayloadList(categoryIndex);
      _lastStatus = String("Category: ") + _categoryNames[categoryIndex];
      _needsRender = true;
    }
    return false;
  }

  if (_selectedPayloadRow == 0) {
    enterCategoryList();
    _lastStatus = "Back to categories";
    _needsRender = true;
    return false;
  }

  const uint8_t payloadVisibleIndex = _selectedPayloadRow - 1;
  if (payloadVisibleIndex >= selectedCategoryPayloadCount()) {
    _lastStatus = "No payload";
    _needsRender = true;
    return false;
  }

  _selectedPayload = payloadVisibleIndex;
  if (!runSelected()) {
    _needsRender = true;
  }
  return false;
}

void HidActions::enterPayloadList(uint8_t categoryIndex) {
  if (_categoryCount == 0) {
    _selectedCategory = 0;
    _selectedPayload = 0;
    _selectedPayloadRow = 0;
    _uiMode = UI_MODE_PAYLOAD_LIST;
    return;
  }

  _selectedCategory = categoryIndex % _categoryCount;
  _selectedPayload = 0;
  const uint8_t payloadCount = selectedCategoryPayloadCount();
  _selectedPayloadRow = payloadCount > 0 ? 1 : 0;
  _uiMode = UI_MODE_PAYLOAD_LIST;

  Serial.printf("HID category selected: %s payloads=%u\n", _categoryNames[_selectedCategory],
                static_cast<unsigned>(payloadCount));
}

void HidActions::enterCategoryList() {
  _uiMode = UI_MODE_CATEGORY_LIST;
  if (_selectedCategoryRow > _categoryCount) {
    _selectedCategoryRow = 0;
  }
}

uint8_t HidActions::findCategoryIndex(const char *category) const {
  for (uint8_t i = 0; i < _categoryCount; i++) {
    if (strcmp(_categoryNames[i], category) == 0) {
      return i;
    }
  }
  return 0xFF;
}

uint8_t HidActions::selectedCategoryPayloadCount() const {
  if (_categoryCount == 0 || _selectedCategory >= _categoryCount) {
    return 0;
  }
  return _categoryCounts[_selectedCategory] > 255 ? 255 : static_cast<uint8_t>(_categoryCounts[_selectedCategory]);
}

uint8_t HidActions::currentSelectionRow() const {
  return (_uiMode == UI_MODE_CATEGORY_LIST) ? _selectedCategoryRow : _selectedPayloadRow;
}

uint8_t HidActions::currentRowCount() const {
  if (_uiMode == UI_MODE_CATEGORY_LIST) {
    return _categoryCount + 1; // includes back row
  }
  return selectedCategoryPayloadCount() + 1; // includes back row
}

void HidActions::stopRun(const char *status) {
#if HID_USBHID_AVAILABLE
  gHidKeyboard.releaseAll();
#endif
  _running = false;
  _execState = EXEC_IDLE;
  _commandIndex = 0;
  _nextActionMs = 0;
  _lastStatus = status;
  _needsRender = true;
}

void HidActions::serviceExecution() {
  if (!_running) {
    return;
  }

  const uint32_t now = millis();
  if (now < _nextActionMs) {
    return;
  }

#if HID_USBHID_AVAILABLE
  const uint16_t payloadIndex = payloadIndexForVisible(_selectedPayload);
  if (payloadIndex >= kHidPayloadCount) {
    stopRun("No payload");
    _uiMode = UI_MODE_PAYLOAD_LIST;
    return;
  }
  const HidPayloadDef &payload = kHidPayloads[payloadIndex];
  const bool iosMode = isIosPlatform(payload.platform);
  _usbReady = (bool)USB;

  switch (_execState) {
  case EXEC_WAIT_USB:
    if (!_usbReady) {
      _lastStatus = "Waiting for USB";
      _nextActionMs = now + 250;
      _needsRender = true;
      break;
    }
    _execState = iosMode ? EXEC_SEND_COMMAND : EXEC_BOOT_WAIT;
    _lastStatus = "USB ready";
    _nextActionMs = now + (iosMode ? 100 : EXEC_BOOT_WAIT_MS);
    _needsRender = true;
    break;
  case EXEC_BOOT_WAIT:
    _execState = EXEC_OPEN_SPOTLIGHT;
    _nextActionMs = now + EXEC_AFTER_BEGIN_MS;
    _lastStatus = "Launching";
    _needsRender = true;
    break;
  case EXEC_OPEN_SPOTLIGHT:
    keyCombo(KEY_LEFT_GUI, ' ');
    _execState = EXEC_TYPE_TERMINAL;
    _nextActionMs = now + EXEC_AFTER_SPOTLIGHT_MS;
    _lastStatus = "Opening launcher";
    _needsRender = true;
    break;
  case EXEC_TYPE_TERMINAL:
    gHidKeyboard.print("terminal");
    _execState = EXEC_OPEN_TERMINAL;
    _nextActionMs = now + EXEC_AFTER_TERMINAL_TEXT_MS;
    _lastStatus = "Typing terminal";
    _needsRender = true;
    break;
  case EXEC_OPEN_TERMINAL:
    pressReturn();
    _execState = EXEC_WAIT_TERMINAL;
    _nextActionMs = now + EXEC_AFTER_TERMINAL_OPEN_MS;
    _lastStatus = "Opening terminal";
    _needsRender = true;
    break;
  case EXEC_WAIT_TERMINAL:
    _execState = EXEC_SEND_COMMAND;
    _nextActionMs = now + 20;
    break;
  case EXEC_SEND_COMMAND:
    if (_commandIndex >= payload.commandCount) {
      _execState = EXEC_DONE;
      _nextActionMs = now + 10;
      break;
    }
    sendCommand(payload.commands[_commandIndex], iosMode);
    _nextActionMs = now + (iosMode ? EXEC_AFTER_COMMAND_MS_IOS : EXEC_AFTER_COMMAND_MS) +
                     sleepMsFromCommand(payload.commands[_commandIndex]);
    _commandIndex++;
    _execState = EXEC_WAIT_AFTER_COMMAND;
    _lastStatus = "Sending command";
    _needsRender = true;
    break;
  case EXEC_WAIT_AFTER_COMMAND:
    _execState = EXEC_SEND_COMMAND;
    _nextActionMs = now + 10;
    break;
  case EXEC_DONE:
    stopRun("Done");
    _uiMode = UI_MODE_PAYLOAD_LIST;
    break;
  case EXEC_FAILED:
    stopRun("Failed");
    _uiMode = UI_MODE_PAYLOAD_LIST;
    break;
  case EXEC_IDLE:
  default:
    break;
  }
#else
  stopRun("HID unavailable");
  _uiMode = UI_MODE_PAYLOAD_LIST;
#endif
}

void HidActions::sendCommand(const char *text, bool iosMode) {
#if HID_USBHID_AVAILABLE
  if (iosMode) {
    if (strncmp(text, "WAIT:", 5) == 0) {
      return;
    }
    if (strncmp(text, "TEXT:", 5) == 0) {
      gHidKeyboard.print(text + 5);
      return;
    }
    if (strncmp(text, "KEY:", 4) == 0) {
      if (!sendIosKeyCombo(text + 4)) {
        _lastStatus = "Bad iOS key";
      }
      return;
    }
    gHidKeyboard.print(text);
    return;
  }
  gHidKeyboard.print(text);
  pressReturn();
#else
  (void)text;
  (void)iosMode;
#endif
}

void HidActions::pressReturn() {
#if HID_USBHID_AVAILABLE
  gHidKeyboard.press(KEY_RETURN);
  delayMicroseconds(700);
  gHidKeyboard.release(KEY_RETURN);
#endif
}

void HidActions::keyCombo(uint8_t mod, uint8_t key) {
#if HID_USBHID_AVAILABLE
  gHidKeyboard.press(mod);
  gHidKeyboard.press(key);
  delayMicroseconds(900);
  gHidKeyboard.releaseAll();
#else
  (void)mod;
  (void)key;
#endif
}

// Returns the largest `sleep N` value found in cmd, converted to milliseconds.
// Used to hold off the next command until the shell sleep actually finishes.
uint32_t HidActions::sleepMsFromCommand(const char *cmd) {
  if (strncmp(cmd, "WAIT:", 5) == 0) {
    long ms = strtol(cmd + 5, nullptr, 10);
    if (ms > 0) {
      return static_cast<uint32_t>(ms);
    }
  }
  const char *p = cmd;
  uint32_t best = 0;
  while ((p = strstr(p, "sleep ")) != nullptr) {
    p += 6;
    char *end;
    long val = strtol(p, &end, 10);
    if (end != p && val > 0) {
      best = max(best, (uint32_t)(val * 1000));
    }
  }
  return best;
}

String HidActions::truncate(const String &input, uint8_t maxLen) {
  if (input.length() <= maxLen) {
    return input;
  }
  if (maxLen < 4) {
    return input.substring(0, maxLen);
  }
  return input.substring(0, maxLen - 3) + "...";
}

bool HidActions::sendIosKeyCombo(const char *combo) {
#if HID_USBHID_AVAILABLE
  String raw = combo;
  raw.trim();
  raw.toUpperCase();

  uint8_t mod = 0;
  uint8_t key = 0;
  int start = 0;
  while (start <= raw.length()) {
    int end = raw.indexOf('+', start);
    if (end < 0) {
      end = raw.length();
    }
    String token = raw.substring(start, end);
    token.trim();
    if (token == "CMD" || token == "GUI") {
      mod |= KEY_LEFT_GUI;
    } else if (token == "SHIFT") {
      mod |= KEY_LEFT_SHIFT;
    } else if (token == "ALT" || token == "OPTION") {
      mod |= KEY_LEFT_ALT;
    } else if (token == "CTRL" || token == "CONTROL") {
      mod |= KEY_LEFT_CTRL;
    } else if (token == "SPACE") {
      key = ' ';
    } else if (token == "TAB") {
      key = KEY_TAB;
    } else if (token == "ENTER" || token == "RETURN") {
      key = KEY_RETURN;
    } else if (token == "H") {
      key = 'h';
    } else if (token == "L") {
      key = 'l';
    } else if (token.length() == 1 && isalpha(token[0])) {
      key = static_cast<uint8_t>(tolower(token[0]));
    } else {
      return false;
    }
    start = end + 1;
  }
  if (key == 0) {
    return false;
  }
  keyCombo(mod, key);
  return true;
#else
  (void)combo;
  return false;
#endif
}

bool HidActions::isIosPlatform(const char *platform) const {
  if (platform == nullptr) {
    return false;
  }
  return strcmp(platform, "ios_shared") == 0 || strcmp(platform, "ios_simple") == 0 ||
         strcmp(platform, "iphone") == 0 || strcmp(platform, "ipad") == 0;
}

uint16_t HidActions::payloadIndexForVisible(uint8_t visibleIndex) const {
  if (_categoryCount == 0 || _selectedCategory >= _categoryCount) {
    return kHidPayloadCount;
  }

  const uint16_t categoryCount = _categoryCounts[_selectedCategory];
  if (visibleIndex >= categoryCount) {
    return kHidPayloadCount;
  }

  const uint16_t slot = _categoryStarts[_selectedCategory] + visibleIndex;
  if (slot >= _categoryPayloadCount || slot >= MAX_CATEGORY_PAYLOAD_INDEXES) {
    return kHidPayloadCount;
  }
  return _categoryPayloadIndexes[slot];
}
