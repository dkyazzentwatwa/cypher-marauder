#include "ScanUi.h"

#include <Adafruit_ST7789.h>

namespace {
constexpr int BODY_TOP = 42;
constexpr int ROW_HEIGHT = 28;
constexpr int BODY_BOTTOM_PAD = 22;

uint32_t mixHash(uint32_t hash, uint32_t value) {
  hash ^= value + 0x9E3779B9UL + (hash << 6) + (hash >> 2);
  return hash;
}

uint32_t stringHash(const String &value) {
  uint32_t hash = 2166136261UL;
  for (size_t i = 0; i < value.length(); i++) {
    hash ^= static_cast<uint8_t>(value[i]);
    hash *= 16777619UL;
  }
  return hash;
}
}

void ScanUi::resetState(ScanUiState &state) {
  state.selected = 0;
  state.detailMode = false;
  state.painted = false;
  state.lastHash = 0;
}

bool ScanUi::handleButton(ScanUiState &state, int itemCount, ButtonEvent event) {
  if (event == BUTTON_HOLD_5S || event == BUTTON_BACK) {
    return true;
  }

  if (itemCount <= 0) {
    return false;
  }

  if (event == BUTTON_CLICK) {
    state.selected = (state.selected + 1) % itemCount;
  } else if (event == BUTTON_PREV) {
    state.selected = (state.selected - 1 + itemCount) % itemCount;
  } else if (event == BUTTON_SELECT) {
    state.detailMode = !state.detailMode;
  } else if (event == BUTTON_HOLD_2S) {
    state.detailMode = !state.detailMode;
  }

  return false;
}

void ScanUi::draw(DisplayPort &display, const char *title, const ScanItem *items, int itemCount,
                  ScanUiState &state, bool scanning, const String &status, ScanSource source) {
  Adafruit_GFX &gfx = display.gfx();

  uint32_t hash = 0x811C9DC5UL;
  hash = mixHash(hash, stringHash(title));
  hash = mixHash(hash, stringHash(status));
  hash = mixHash(hash, static_cast<uint32_t>(itemCount));
  hash = mixHash(hash, static_cast<uint32_t>(state.selected));
  hash = mixHash(hash, state.detailMode ? 1 : 0);
  hash = mixHash(hash, scanning ? 1 : 0);
  hash = mixHash(hash, static_cast<uint32_t>(source));
  const int selected = itemCount > 0 ? constrain(state.selected, 0, itemCount - 1) : 0;
  const int rows = rowsPerPage(display);
  const int page = pageForIndex(display, selected);
  const int first = page * rows;
  const int last = min(itemCount, first + rows);
  if (state.detailMode && itemCount > 0) {
    const ScanItem &item = items[selected];
    hash = mixHash(hash, stringHash(item.name));
    hash = mixHash(hash, stringHash(item.id));
    hash = mixHash(hash, static_cast<uint32_t>(item.rssi + 200));
    hash = mixHash(hash, static_cast<uint32_t>(item.channel));
    hash = mixHash(hash, item.secure ? 1 : 0);
    hash = mixHash(hash, item.connectable ? 1 : 0);
    hash = mixHash(hash, item.hasTxPower ? 1 : 0);
    hash = mixHash(hash, static_cast<uint32_t>(item.txPower + 200));
  } else {
    for (int i = first; i < last; i++) {
      hash = mixHash(hash, stringHash(items[i].name));
      hash = mixHash(hash, stringHash(items[i].id));
      hash = mixHash(hash, static_cast<uint32_t>(items[i].rssi + 200));
      hash = mixHash(hash, static_cast<uint32_t>(items[i].channel));
      hash = mixHash(hash, items[i].secure ? 1 : 0);
      hash = mixHash(hash, items[i].connectable ? 1 : 0);
    }
  }

  if (state.painted && state.lastHash == hash) {
    return;
  }
  state.painted = true;
  state.lastHash = hash;

  display.clear();
  display.drawHeader(title);

  gfx.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  gfx.setCursor(4, 26);
  String head = status;
  if (head.length() == 0) {
    head = scanning ? "Scanning..." : "Idle";
  }
  head += " n=";
  head += itemCount;
  gfx.print(truncateText(head, 27));

  if (itemCount <= 0) {
    display.drawFooter("Hold5 back");
    return;
  }

  if (state.detailMode) {
    const ScanItem &item = items[selected];
    gfx.setCursor(4, BODY_TOP);
    gfx.print("Item ");
    gfx.print(selected + 1);
    gfx.print("/");
    gfx.print(itemCount);

    int y = BODY_TOP + 16;
    gfx.setCursor(4, y);
    gfx.print(truncateText(item.name, 27));
    y += 14;

    gfx.setCursor(4, y);
    gfx.print(truncateText(item.id, 27));
    y += 14;

    gfx.setCursor(4, y);
    gfx.print("RSSI ");
    gfx.print(item.rssi);
    if (item.hasChannel) {
      gfx.print("  CH ");
      gfx.print(item.channel);
    }
    y += 14;

    if (source == SCAN_SOURCE_WIFI) {
      gfx.setCursor(4, y);
      gfx.print("SEC ");
      gfx.print(item.secure ? "Yes" : "Open");
    } else {
      gfx.setCursor(4, y);
      gfx.print("CONN ");
      gfx.print(item.connectable ? "Yes" : "No");
      if (item.hasTxPower) {
        gfx.print(" TX ");
        gfx.print(item.txPower);
      }
    }

    display.drawFooter("Click next Hold2 list 5s back");
    return;
  }

  for (int i = first; i < last; i++) {
    const int row = i - first;
    const int y = BODY_TOP + row * ROW_HEIGHT;
    const bool isSelected = (i == selected);
    const uint16_t bg = isSelected ? ST77XX_CYAN : ST77XX_BLACK;
    const uint16_t fg = isSelected ? ST77XX_BLACK : ST77XX_WHITE;

    gfx.fillRoundRect(2, y, display.width() - 4, ROW_HEIGHT - 2, 4, bg);
    gfx.setTextColor(fg, bg);

    String prefix;
    prefix += String(items[i].rssi);
    prefix += " ";
    if (source == SCAN_SOURCE_WIFI) {
      prefix += "C";
      if (items[i].channel < 10) {
        prefix += "0";
      }
      prefix += String(items[i].channel);
      prefix += " ";
      prefix += items[i].secure ? "S " : "O ";
    } else {
      prefix += items[i].connectable ? "C " : "- ";
    }

    const int nameChars = 18;
    const String line = prefix + truncateText(items[i].name, nameChars);
    gfx.setCursor(6, y + 8);
    gfx.print(line);
  }

  String pageText = "P";
  pageText += (page + 1);
  pageText += "/";
  pageText += ((itemCount + rows - 1) / rows);
  display.drawFooter((String("Click next Hold2 det 5s back ") + pageText).c_str());
}

String ScanUi::truncateText(const String &in, int maxChars) {
  if (in.length() <= maxChars) {
    return in;
  }
  if (maxChars <= 3) {
    return in.substring(0, maxChars);
  }
  return in.substring(0, maxChars - 3) + "...";
}

int ScanUi::rowsPerPage(const DisplayPort &display) {
  const int rows = (display.height() - BODY_TOP - BODY_BOTTOM_PAD) / ROW_HEIGHT;
  return max(1, rows);
}

int ScanUi::pageForIndex(const DisplayPort &display, int index) {
  const int rows = rowsPerPage(display);
  return index / rows;
}
