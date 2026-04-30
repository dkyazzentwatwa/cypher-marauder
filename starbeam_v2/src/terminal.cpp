// terminal.cpp - Serial Terminal/CLI Implementation for Project Starbeam V2
// Provides command-line control via serial port for headless operation

#include "terminal.h"
#include "display.h"
#include "wifi_scanner.h"
#include "ble_scanner.h"
#include "MarauderCore.h"

// ============================================================================
// Static Member Initialization
// ============================================================================

char Terminal::lineBuffer[BUF_LENGTH];
uint8_t Terminal::bufferIndex = 0;
bool Terminal::commandReady = false;
MenuItem Terminal::pendingCommand = BT_JAM;  // Default value
bool Terminal::stopFlag = false;
bool Terminal::echoEnabled = true;  // Echo on by default
bool Terminal::verboseLogging = false;
bool Terminal::initialized = false;

// ============================================================================
// Command Mapping Table
// Maps snake_case commands to MenuItem enum values
// ============================================================================

static const CommandMapping commands[] = {
    // Jamming Operations
    {"bt_jam", BT_JAM, "Bluetooth jammer (2.4GHz)"},
    {"drone_jam", DRONE_JAM, "Drone jammer"},
    {"wifi_jam", WIFI_JAM, "WiFi jammer (2.4GHz)"},
    {"cc1_jam", CC1_JAM, "433MHz jammer (both radios)"},

    // Scanning Operations
    {"cc_scan", CC_SCAN, "CC1101 frequency scan"},
    {"nrf_scan", NRF_SCAN, "NRF24 spectrum analyzer"},
    {"wifi_scan", WIFI_SCAN, "WiFi network scanner"},
    {"wifi_heatmap", WIFI_HEATMAP, "WiFi channel heatmap"},
    {"ble_scan", BLE_SCAN, "BLE device scanner"},

    // Web Server
    {"web_on", WEBSERVER_ON, "Start web server"},
    {"web_off", WEBSERVER_OFF, "Stop web server"},
    {"web_status", WEBSERVER_STATUS, "Web server status"},

    // WiFi Security Testing
    {"deauth_target", SEC_DEAUTH_TARGET, "Targeted deauth attack"},
    {"deauth_all", SEC_DEAUTH_ALL, "Broadcast deauth attack"},
    {"beacon_flood", SEC_BEACON_FLOOD, "Beacon flooding attack"},
    {"probe_flood", SEC_PROBE_FLOOD, "Probe request flooding"},
    {"pmkid", SEC_PMKID_CAPTURE, "PMKID capture"},

    // Hardware Tests
    {"test_nrf", TEST_NRF, "Test NRF24 radios (VSPI)"},
    {"test_cc", TEST_CC1101, "Test CC1101 radios"},
    {"test_hspi", TEST_HSPI, "Test NRF24 radios (HSPI)"},

    // CC1101 Operations
    {"cc1_single", CC1_SINGLE, "CC1101 #1 only jammer"},
    {"cc2_single", CC2_SINGLE, "CC1101 #2 only jammer"},

    // Recording Operations
    {"rec_raw", REC_RAW, "Record raw 433MHz signal"},
    {"play_raw", PLAY_RAW, "Replay raw 433MHz signal"},
    {"show_raw", SHOW_RAW, "Show raw recording data"},
    {"show_buff", SHOW_BUFF, "Show buffer contents"},

    // Utility Operations
    {"get_rssi", GET_RSSI, "Get CC1101 RSSI value"},
    {"flush_buff", FLUSH_BUFF, "Clear recording buffer"},
    {"stop_all", STOP_ALL, "Stop all operations"},
    {"reset_cc", RESET_CC, "Reset CC1101 radios"},

    // Frequency Settings
    {"freq_43440", SET_43440, "Set frequency 434.40MHz"},
    {"freq_43430", SET_43430, "Set frequency 434.30MHz"},
    {"freq_43400", SET_43400, "Set frequency 434.00MHz"},
    {"freq_43390", SET_43390, "Set frequency 433.90MHz"},

    // System
    {"settings", SETTINGS, "Settings menu"},
    {"help", HELP, "Show help"}
};

static const int numCommands = sizeof(commands) / sizeof(commands[0]);

// ============================================================================
// Public Functions
// ============================================================================

void Terminal::init() {
    if (initialized) return;

    // Clear buffer
    bufferIndex = 0;
    lineBuffer[0] = '\0';

    // Reset flags
    commandReady = false;
    stopFlag = false;
    echoEnabled = true;
    verboseLogging = false;

    // Print welcome message
    Serial.println();
    Serial.println("========================================");
    Serial.println("  STARBEAM V2 TERMINAL");
    Serial.println("  Type 'help' for commands");
    Serial.println("========================================");
    Serial.print("> ");

    MarauderCore::core().begin("starbeam-v2");
    initialized = true;
}

void Terminal::processInput() {
    MarauderCore::core().poll();

    // Process all available serial characters
    while (Serial.available() > 0) {
        char c = Serial.read();

        // Handle backspace/delete
        if (c == 8 || c == 127) {
            if (bufferIndex > 0) {
                bufferIndex--;
                Serial.write("\b \b");  // Erase character on terminal
            }
            continue;
        }

        // Handle newline/carriage return
        if (c == '\n' || c == '\r') {
            Serial.println();  // Echo newline
            if (bufferIndex > 0) {
                lineBuffer[bufferIndex] = '\0';
                parseCommand(String(lineBuffer));
                bufferIndex = 0;
            }
            Serial.print("> ");  // Show prompt
            continue;
        }

        // Accumulate printable characters
        if (c >= 32 && c < 127) {  // Printable ASCII range
            if (bufferIndex < BUF_LENGTH - 1) {
                lineBuffer[bufferIndex++] = c;
                Serial.write(c);  // Echo character
            }
            // Silently ignore if buffer full
        }
    }
}

bool Terminal::hasCommand() {
    return commandReady;
}

MenuItem Terminal::getCommand() {
    return pendingCommand;
}

void Terminal::clearCommand() {
    commandReady = false;
}

bool Terminal::stopRequested() {
    return stopFlag;
}

void Terminal::clearStopFlag() {
    stopFlag = false;
}

void Terminal::echoToSerial(const String& line1, const String& line2,
                           const String& line3, const String& line4) {
    if (!echoEnabled) return;

    // Clean text output for terminal (no box characters)
    Serial.println();
    Serial.println("========================");
    if (line1.length() > 0) Serial.println(line1);
    if (line2.length() > 0) Serial.println(line2);
    if (line3.length() > 0) Serial.println(line3);
    if (line4.length() > 0) Serial.println(line4);
    Serial.println("========================");
}

// ============================================================================
// Private Functions - Command Parsing
// ============================================================================

void Terminal::parseCommand(const String& cmd) {
    String trimmed = cmd;
    trimmed.trim();
    trimmed.toLowerCase();

    if (trimmed.length() == 0) {
        return;  // Empty command, just show prompt again
    }

    if (MarauderCore::core().handleCommand(trimmed)) {
        return;
    }

    // ========================================================================
    // Special Commands (not in menu system)
    // ========================================================================

    if (trimmed == "help" || trimmed == "?") {
        printHelp();
        return;
    }

    if (trimmed == "status") {
        printStatus();
        return;
    }

    if (trimmed == "menu") {
        printMenu();
        return;
    }

    if (trimmed == "stop") {
        stopFlag = true;
        Serial.println("Stop signal sent");
        return;
    }

    if (trimmed.startsWith("echo ")) {
        String arg = trimmed.substring(5);
        arg.trim();
        if (arg == "on") {
            echoEnabled = true;
            Serial.println("Display echo: ON");
        } else if (arg == "off") {
            echoEnabled = false;
            Serial.println("Display echo: OFF");
        } else {
            Serial.println("Usage: echo on|off");
        }
        return;
    }

    if (trimmed.startsWith("verbose ")) {
        String arg = trimmed.substring(8);
        arg.trim();
        if (arg == "on") {
            verboseLogging = true;
            Serial.println("Verbose logging: ON");
        } else if (arg == "off") {
            verboseLogging = false;
            Serial.println("Verbose logging: OFF");
        } else {
            Serial.println("Usage: verbose on|off");
        }
        return;
    }

    if (trimmed == "wifi_list") {
        WiFiScanner::listAll();
        return;
    }

    if (trimmed == "ble_list") {
        BLEScanner::listAll();
        return;
    }

    // ========================================================================
    // Menu Commands (look up in command table)
    // ========================================================================

    MenuItem item = commandToMenuItem(trimmed);
    if (item != (MenuItem)-1) {
        pendingCommand = item;
        commandReady = true;
        Serial.printf("Command accepted: %s\n", trimmed.c_str());
    } else {
        Serial.printf("Unknown command: '%s' (type 'help')\n", trimmed.c_str());
    }
}

MenuItem Terminal::commandToMenuItem(const String& cmd) {
    // Linear search through command table
    for (int i = 0; i < numCommands; i++) {
        if (cmd.equals(commands[i].command)) {
            return commands[i].menuItem;
        }
    }

    return (MenuItem)-1;  // Not found
}

// ============================================================================
// Private Functions - Special Command Handlers
// ============================================================================

void Terminal::printHelp() {
    Serial.println();
    Serial.println("========================================");
    Serial.println("  STARBEAM V2 COMMAND REFERENCE");
    Serial.println("========================================");
    Serial.println();
    Serial.println("JAMMING OPERATIONS:");
    Serial.println("  bt_jam            - Bluetooth jammer (2.4GHz)");
    Serial.println("  drone_jam         - Drone jammer");
    Serial.println("  wifi_jam          - WiFi jammer (2.4GHz)");
    Serial.println("  cc1_jam           - 433MHz jammer (both radios)");
    Serial.println("  cc1_single        - CC1101 #1 only jammer");
    Serial.println("  cc2_single        - CC1101 #2 only jammer");
    Serial.println();
    Serial.println("SCANNING OPERATIONS:");
    Serial.println("  cc_scan           - CC1101 frequency scan");
    Serial.println("  nrf_scan          - NRF24 spectrum analyzer");
    Serial.println("  wifi_scan         - WiFi network scanner");
    Serial.println("  wifi_list         - List all WiFi networks");
    Serial.println("  wifi_heatmap      - WiFi channel heatmap");
    Serial.println("  ble_scan          - BLE device scanner");
    Serial.println("  ble_list          - List all BLE devices");
    Serial.println();
    Serial.println("WEB SERVER:");
    Serial.println("  web_on            - Start web server");
    Serial.println("  web_off           - Stop web server");
    Serial.println("  web_status        - Web server status");
    Serial.println();
    Serial.println("SECURITY TESTING:");
    Serial.println("  deauth_target     - Targeted deauth attack");
    Serial.println("  deauth_all        - Broadcast deauth attack");
    Serial.println("  beacon_flood      - Beacon flooding attack");
    Serial.println("  probe_flood       - Probe request flooding");
    Serial.println("  pmkid             - PMKID capture");
    Serial.println();
    Serial.println("HARDWARE TESTS:");
    Serial.println("  test_nrf          - Test NRF24 radios (VSPI)");
    Serial.println("  test_cc           - Test CC1101 radios");
    Serial.println("  test_hspi         - Test NRF24 radios (HSPI)");
    Serial.println();
    Serial.println("RECORDING:");
    Serial.println("  rec_raw           - Record raw 433MHz signal");
    Serial.println("  play_raw          - Replay raw 433MHz signal");
    Serial.println("  show_raw          - Show raw recording data");
    Serial.println("  show_buff         - Show buffer contents");
    Serial.println();
    Serial.println("UTILITIES:");
    Serial.println("  get_rssi          - Get CC1101 RSSI value");
    Serial.println("  flush_buff        - Clear recording buffer");
    Serial.println("  stop_all          - Stop all operations");
    Serial.println("  reset_cc          - Reset CC1101 radios");
    Serial.println();
    Serial.println("FREQUENCY SETTINGS:");
    Serial.println("  freq_43440        - Set frequency 434.40MHz");
    Serial.println("  freq_43430        - Set frequency 434.30MHz");
    Serial.println("  freq_43400        - Set frequency 434.00MHz");
    Serial.println("  freq_43390        - Set frequency 433.90MHz");
    Serial.println();
    Serial.println("SYSTEM:");
    Serial.println("  settings          - Settings menu");
    Serial.println("  help, ?           - Show this help");
    Serial.println("  status            - System status");
    Serial.println("  menu              - List menu items");
    Serial.println("  stop              - Stop current operation");
    Serial.println("  echo on|off       - Toggle display echo");
    Serial.println("  verbose on|off    - Toggle verbose logging");
    Serial.println();
    Serial.println("USAGE:");
    Serial.println("  Type command and press ENTER");
    Serial.println("  Use 'stop' to interrupt operations");
    Serial.println("  Physical buttons also work (last wins)");
    Serial.println("========================================");
    Serial.println();
}

void Terminal::printStatus() {
    Serial.println();
    Serial.println("========================================");
    Serial.println("  SYSTEM STATUS");
    Serial.println("========================================");
    Serial.printf("Free Heap:     %lu bytes\n", ESP.getFreeHeap());
    Serial.printf("Uptime:        %lu seconds\n", millis() / 1000);
    Serial.printf("Display Echo:  %s\n", echoEnabled ? "ON" : "OFF");
    Serial.printf("Verbose:       %s\n", verboseLogging ? "ON" : "OFF");
    Serial.println();
    Serial.println("Note: State and radio info depend on");
    Serial.println("      current operation mode");
    Serial.println("========================================");
    Serial.println();
}

void Terminal::printMenu() {
    Serial.println();
    Serial.println("========================================");
    Serial.println("  MENU ITEMS (0-35)");
    Serial.println("========================================");

    // Print command table with indices
    for (int i = 0; i < numCommands; i++) {
        Serial.printf("%2d. %-18s - %s\n",
                     i,
                     commands[i].command,
                     commands[i].description);
    }

    Serial.println("========================================");
    Serial.println();
}
