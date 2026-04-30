#include "SDCardBrowser.h"

void SDCardBrowser::begin() {
  _active = true;
  _fileIndex = 0;
  _lineIndex = 0;
  _currentFileContent = "";
  refreshFileList();
}

void SDCardBrowser::refreshFileList() {
  if (!SDCardManager::instance().isAvailable()) {
    _fileCount = 0;
    return;
  }

  _fileCount = SDCardManager::instance().listFiles("/captures", _files, 20);
  if (_fileCount == 0) {
    _currentFileName = "(No captures)";
  }
}

void SDCardBrowser::loadFile(int idx) {
  if (idx < 0 || idx >= _fileCount) {
    return;
  }

  _fileIndex = idx;
  _lineIndex = 0;

  char filePath[128];
  snprintf(filePath, sizeof(filePath), "/captures/%s", _files[idx].name);

  _currentFileName = _files[idx].name;
  _currentFileContent = SDCardManager::instance().readFile(filePath);
}

void SDCardBrowser::renderFileList(DisplayPort &display) {
  Adafruit_GFX &gfx = display.gfx();

  display.clear();
  display.drawHeader(" SD Card Captures");

  if (_fileCount == 0) {
    gfx.setTextColor(ST77XX_WHITE);
    gfx.setCursor(10, 60);
    gfx.println("No captures on SD card");
    gfx.setCursor(10, 90);
    gfx.println("Use Captive Portal to");
    gfx.setCursor(10, 110);
    gfx.println("create captures.");
    display.drawFooter("  Hold select: exit");
    return;
  }

  const int rowH = 24;
  const int startY = 32;
  const int maxRows = 10;

  for (int i = 0; i < maxRows && (_fileIndex + i) < _fileCount; i++) {
    const int y = startY + i * rowH;
    const bool selected = i == 0;

    if (selected) {
      gfx.fillRect(0, y - 2, display.width(), rowH, ST77XX_CYAN);
      gfx.setTextColor(ST77XX_BLACK, ST77XX_CYAN);
    } else {
      gfx.setTextColor(ST77XX_WHITE);
    }

    gfx.setCursor(8, y + 4);
    gfx.print(_files[_fileIndex + i].name);
  }

  gfx.setTextColor(ST77XX_WHITE);
  char footerBuf[64];
  snprintf(footerBuf, sizeof(footerBuf), "  Click: view  Prev: %d/%d", _fileIndex + 1, _fileCount);
  display.drawFooter(footerBuf);
}

void SDCardBrowser::renderFileContent(DisplayPort &display) {
  Adafruit_GFX &gfx = display.gfx();

  display.clear();
  display.drawHeader(" View Capture");

  if (_currentFileContent.length() == 0) {
    gfx.setTextColor(ST77XX_WHITE);
    gfx.setCursor(10, 60);
    gfx.println("Unable to read file");
    display.drawFooter("  Click: back");
    return;
  }

  // Parse CSV and display lines
  const int startY = 32;
  const int lineH = 16;
  const int maxLines = 16;

  int lineNum = 0;
  int currentLine = 0;
  int pos = 0;

  gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  gfx.setTextSize(1);

  while (pos < _currentFileContent.length() && currentLine < maxLines) {
    if (lineNum < _lineIndex) {
      // Skip lines before our view
      while (pos < _currentFileContent.length() && _currentFileContent[pos] != '\n') {
        pos++;
      }
      if (pos < _currentFileContent.length()) {
        pos++; // skip newline
      }
      lineNum++;
      continue;
    }

    // Extract line
    String line = "";
    while (pos < _currentFileContent.length() && _currentFileContent[pos] != '\n') {
      line += _currentFileContent[pos];
      pos++;
    }
    if (pos < _currentFileContent.length()) {
      pos++; // skip newline
    }

    // Truncate long lines for display
    if (line.length() > 25) {
      line = line.substring(0, 22) + "...";
    }

    const int y = startY + currentLine * lineH;
    gfx.setCursor(4, y);
    gfx.println(line);

    currentLine++;
    lineNum++;
  }

  char footerBuf[64];
  snprintf(footerBuf, sizeof(footerBuf), "  Click: list  Scroll: line %d", _lineIndex);
  display.drawFooter(footerBuf);
}

void SDCardBrowser::update(DisplayPort &display) {
  if (!_active) {
    return;
  }

  if (_currentFileContent.length() == 0) {
    renderFileList(display);
  } else {
    renderFileContent(display);
  }
}

bool SDCardBrowser::handleButton(ButtonEvent event) {
  if (!_active) {
    return false;
  }

  if (_currentFileContent.length() == 0) {
    // File list view
    if (event == BUTTON_CLICK) {
      loadFile(_fileIndex);
      return false;
    } else if (event == BUTTON_PREV) {
      _fileIndex = (_fileIndex + 1) % max(1, _fileCount);
      return false;
    } else if (event == BUTTON_HOLD_5S || event == BUTTON_BACK) {
      _active = false;
      return true;
    }
  } else {
    // File content view
    if (event == BUTTON_CLICK) {
      // Return to file list
      _currentFileContent = "";
      _lineIndex = 0;
      return false;
    } else if (event == BUTTON_PREV) {
      // Scroll down
      _lineIndex++;
      return false;
    } else if (event == BUTTON_HOLD_5S || event == BUTTON_BACK) {
      _active = false;
      _currentFileContent = "";
      return true;
    }
  }

  return false;
}
