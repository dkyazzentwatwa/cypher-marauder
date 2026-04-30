#include "SDCardManager.h"
#include "../include/BoardConfig.h"
#include <time.h>

SDCardManager::SDCardManager() {}

SDCardManager::~SDCardManager() {
  stop();
}

bool SDCardManager::begin() {
  // Initialize SD card with native 4-bit mode (hardwired to GPIO13-18)
  // No custom pins needed - ESP32-S3 uses native SD/SDIO interface
  if (!SD.begin()) {
    setError("SD card init failed");
    return false;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    setError("No SD card detected");
    return false;
  }

  // Create captures directory if it doesn't exist
  if (!mkdir("/captures")) {
    // Directory may already exist, which is fine
  }

  _available = true;
  _errorMsg = "OK";
  Serial.printf("SD card initialized: %d bytes total\n", SD.cardSize());
  return true;
}

void SDCardManager::stop() {
  if (_available) {
    SD.end();
    _available = false;
  }
}

bool SDCardManager::mkdir(const char* path) {
  if (!_available) {
    setError("SD card not available");
    return false;
  }

  if (SD.exists(path)) {
    return true;  // Already exists
  }

  if (!SD.mkdir(path)) {
    setError("Failed to create directory");
    return false;
  }

  return true;
}

bool SDCardManager::fileExists(const char* path) const {
  if (!_available) {
    return false;
  }
  return SD.exists(path);
}

bool SDCardManager::writeCapture(const char* directory, const char* templateName,
                                  const char* formData, unsigned long timestamp,
                                  const char* ipAddress) {
  if (!_available) {
    setError("SD card not available");
    return false;
  }

  // Create directory if needed
  if (!mkdir(directory)) {
    return false;
  }

  // Generate CSV filename with timestamp
  char filename[128];
  time_t now = timestamp / 1000;  // Convert millis to seconds
  struct tm* timeinfo = localtime(&now);
  snprintf(filename, sizeof(filename), "%s/%04d%02d%02d_%02d%02d%02d.csv",
           directory,
           timeinfo->tm_year + 1900,
           timeinfo->tm_mon + 1,
           timeinfo->tm_mday,
           timeinfo->tm_hour,
           timeinfo->tm_min,
           timeinfo->tm_sec);

  // Open file in append mode
  File file = SD.open(filename, FILE_APPEND);
  if (!file) {
    setError("Failed to open file");
    return false;
  }

  // Write CSV header if file is new (size = 0)
  if (file.size() == 0) {
    file.println("timestamp,template,form_data,ip_address");
  }

  // Write CSV data row
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "%lu,%s,%s,%s",
           timestamp, templateName, formData, ipAddress);
  file.println(buffer);

  file.close();
  return true;
}

int SDCardManager::listFiles(const char* path, FileInfo* outFiles, int maxFiles) {
  if (!_available || !outFiles || maxFiles <= 0) {
    return 0;
  }

  if (!SD.exists(path)) {
    setError("Path does not exist");
    return 0;
  }

  File dir = SD.open(path);
  if (!dir || !dir.isDirectory()) {
    setError("Not a directory");
    return 0;
  }

  int count = 0;
  File file = dir.openNextFile();

  while (file && count < maxFiles) {
    snprintf(outFiles[count].name, sizeof(outFiles[count].name), "%s", file.name());
    outFiles[count].size = file.size();
    outFiles[count].isDirectory = file.isDirectory();
    count++;
    file = dir.openNextFile();
  }

  dir.close();
  return count;
}

String SDCardManager::readFile(const char* path) {
  String content = "";

  if (!_available) {
    setError("SD card not available");
    return content;
  }

  if (!SD.exists(path)) {
    setError("File does not exist");
    return content;
  }

  File file = SD.open(path, FILE_READ);
  if (!file) {
    setError("Failed to open file");
    return content;
  }

  // Read up to 16KB to avoid memory issues
  const size_t maxRead = 16384;
  size_t bytesRead = 0;

  while (file.available() && bytesRead < maxRead) {
    content += (char)file.read();
    bytesRead++;
  }

  file.close();
  return content;
}

void SDCardManager::setError(const char* msg) {
  _errorMsg = msg;
  Serial.printf("SDCardManager error: %s\n", msg);
}
