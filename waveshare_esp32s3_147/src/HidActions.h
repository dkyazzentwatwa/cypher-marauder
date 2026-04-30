#pragma once

#include <Arduino.h>

#include "ButtonHandler.h"
#include "DisplayPort.h"
#include "HidPayloadsGenerated.h"

class HidActions {
public:
  void begin();
  bool handleButton(ButtonEvent event);
  void update(DisplayPort &display, bool drawToScreen);

  void setSelected(uint8_t index);
  uint8_t selected() const;
  uint8_t count() const;

  bool runSelected();
  void stop();
  bool isRunning() const;
  String lastStatus() const;
  bool available() const;
  uint8_t categoryCount() const;
  String categoryName(uint8_t index) const;
  bool selectCategoryByIndex(uint8_t index);
  bool selectCategoryByName(const String &name);
  uint8_t payloadCountInSelectedCategory() const;
  int selectedCategoryIndex() const;
  String selectedCategoryName() const;
  bool selectPayloadByVisibleIndex(uint8_t index);
  const HidPayloadDef *payloadForVisibleIndex(uint8_t index) const;
  const HidPayloadDef *selectedPayloadDef() const;

private:
  static constexpr uint8_t MAX_CATEGORIES = 64;
  static constexpr uint16_t MAX_CATEGORY_PAYLOAD_INDEXES = 256;

  enum UiMode {
    UI_MODE_CATEGORY_LIST,
    UI_MODE_PAYLOAD_LIST,
    UI_MODE_RUNNING
  };

  enum ExecState {
    EXEC_IDLE,
    EXEC_WAIT_USB,
    EXEC_BOOT_WAIT,
    EXEC_OPEN_SPOTLIGHT,
    EXEC_TYPE_TERMINAL,
    EXEC_OPEN_TERMINAL,
    EXEC_WAIT_TERMINAL,
    EXEC_SEND_COMMAND,
    EXEC_WAIT_AFTER_COMMAND,
    EXEC_DONE,
    EXEC_FAILED
  };

  void rebuildCategoryPayloadMap();
  void draw(DisplayPort &display);
  void moveSelection(int8_t delta);
  bool selectCurrent();
  void enterPayloadList(uint8_t categoryIndex);
  void enterCategoryList();
  uint8_t findCategoryIndex(const char *category) const;
  uint8_t selectedCategoryPayloadCount() const;
  uint8_t currentSelectionRow() const;
  uint8_t currentRowCount() const;
  void stopRun(const char *status);
  void serviceExecution();
  void sendCommand(const char *text, bool iosMode);
  void pressReturn();
  void keyCombo(uint8_t mod, uint8_t key);
  bool sendIosKeyCombo(const char *combo);
  bool isIosPlatform(const char *platform) const;
  uint16_t payloadIndexForVisible(uint8_t visibleIndex) const;
  static uint32_t sleepMsFromCommand(const char *cmd);
  static String truncate(const String &input, uint8_t maxLen);

  UiMode _uiMode = UI_MODE_CATEGORY_LIST;
  bool _available = false;
  bool _usbStarted = false;
  bool _usbReady = false;
  bool _running = false;
  uint8_t _selectedCategory = 0;
  uint8_t _selectedCategoryRow = 0;
  uint8_t _selectedPayloadRow = 0;
  uint8_t _selectedPayload = 0;
  uint8_t _commandIndex = 0;
  const char *_categoryNames[MAX_CATEGORIES];
  uint16_t _categoryStarts[MAX_CATEGORIES];
  uint16_t _categoryCounts[MAX_CATEGORIES];
  uint8_t _categoryCount = 0;
  uint16_t _categoryPayloadIndexes[MAX_CATEGORY_PAYLOAD_INDEXES];
  uint16_t _categoryPayloadCount = 0;
  ExecState _execState = EXEC_IDLE;
  uint32_t _nextActionMs = 0;
  uint32_t _lastRunMs = 0;
  bool _needsRender = true;
  bool _wasVisible = false;
  String _lastStatus = "Idle";
};
