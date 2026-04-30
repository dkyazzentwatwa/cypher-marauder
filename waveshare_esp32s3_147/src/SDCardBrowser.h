#pragma once

#include <Arduino.h>
#include "DisplayPort.h"
#include "ButtonHandler.h"
#include "SDCardManager.h"

class SDCardBrowser {
public:
  void begin();
  void update(DisplayPort &display);
  bool handleButton(ButtonEvent event);
  bool active() const { return _active; }

private:
  // States
  bool _active = false;
  int _fileIndex = 0;
  int _lineIndex = 0;

  // Current file data
  String _currentFileName = "";
  String _currentFileContent = "";

  // File browsing
  SDCardManager::FileInfo _files[20];
  int _fileCount = 0;

  void refreshFileList();
  void loadFile(int idx);
  void renderFileList(DisplayPort &display);
  void renderFileContent(DisplayPort &display);
};
