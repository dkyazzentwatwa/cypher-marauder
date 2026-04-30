#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SD.h>

class SDCardManager {
public:
  static SDCardManager& instance() {
    static SDCardManager inst;
    return inst;
  }

  // Initialize SD card with native 4-bit mode pins
  bool begin();
  void stop();
  bool isAvailable() const { return _available; }

  // File operations
  bool mkdir(const char* path);
  bool fileExists(const char* path) const;

  // CSV operations for captive portal
  bool writeCapture(const char* directory, const char* templateName,
                    const char* formData, unsigned long timestamp, const char* ipAddress);

  // File browsing
  struct FileInfo {
    char name[64];
    size_t size;
    bool isDirectory;
  };

  int listFiles(const char* path, FileInfo* outFiles, int maxFiles);
  String readFile(const char* path);

  // Utility
  const char* getErrorMessage() const { return _errorMsg.c_str(); }

private:
  SDCardManager();
  ~SDCardManager();

  bool _available = false;
  String _errorMsg = "";

  void setError(const char* msg);
};
