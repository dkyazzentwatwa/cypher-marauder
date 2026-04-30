// terminal.h - Serial Terminal/CLI Interface for Project Starbeam V2
// Provides command-line control of all menu functions via serial port
// Enables headless operation without physical buttons or display

#ifndef TERMINAL_H
#define TERMINAL_H

#include <Arduino.h>
#include "../types.h"
#include "../config.h"

// Command mapping structure
struct CommandMapping {
    const char* command;       // Command string (e.g., "wifi_scan")
    MenuItem menuItem;         // Corresponding menu item enum
    const char* description;   // Help text description
};

class Terminal {
public:
    // ========================================================================
    // Lifecycle
    // ========================================================================

    /**
     * Initialize terminal interface
     * Call once in setup() after Serial.begin()
     */
    static void init();

    // ========================================================================
    // Input Processing (call in main loop)
    // ========================================================================

    /**
     * Process serial input in non-blocking manner
     * Accumulates characters into line buffer until newline received
     * Call this every loop() iteration for responsiveness
     */
    static void processInput();

    // ========================================================================
    // Command Execution Interface
    // ========================================================================

    /**
     * Check if a command is ready to execute
     * @return true if command has been parsed and is pending execution
     */
    static bool hasCommand();

    /**
     * Get the pending command
     * @return MenuItem corresponding to the command entered
     */
    static MenuItem getCommand();

    /**
     * Clear the command ready flag
     * Call after executing the command
     */
    static void clearCommand();

    // ========================================================================
    // Stop Signal (for interrupting operations)
    // ========================================================================

    /**
     * Check if user requested stop via 'stop' command
     * @return true if stop was requested
     */
    static bool stopRequested();

    /**
     * Clear the stop flag
     * Call after handling the stop request
     */
    static void clearStopFlag();

    // ========================================================================
    // Display Echo (mirror OLED to serial)
    // ========================================================================

    /**
     * Echo display content to serial terminal
     * Formats output to match OLED layout with border
     * @param line1 First line of display (title)
     * @param line2 Second line of display
     * @param line3 Third line of display
     * @param line4 Fourth line of display
     */
    static void echoToSerial(const String& line1, const String& line2,
                            const String& line3, const String& line4);

    /**
     * Set display echo enabled/disabled
     * @param enabled true to echo display to serial, false to disable
     */
    static void setEchoEnabled(bool enabled) { echoEnabled = enabled; }

    /**
     * Check if display echo is enabled
     * @return true if echo is on
     */
    static bool isEchoEnabled() { return echoEnabled; }

    // ========================================================================
    // Verbose Logging
    // ========================================================================

    /**
     * Set verbose logging mode
     * @param enabled true for verbose output, false for minimal
     */
    static void setVerbose(bool enabled) { verboseLogging = enabled; }

    /**
     * Check if verbose logging is enabled
     * @return true if verbose mode is on
     */
    static bool isVerbose() { return verboseLogging; }

private:
    // ========================================================================
    // Command Parsing
    // ========================================================================

    /**
     * Parse a complete command line
     * Handles special commands (help, status, stop, echo)
     * and looks up menu commands in command table
     * @param cmd Command string to parse
     */
    static void parseCommand(const String& cmd);

    /**
     * Look up MenuItem corresponding to command string
     * @param cmd Command string to look up
     * @return MenuItem enum or (MenuItem)-1 if not found
     */
    static MenuItem commandToMenuItem(const String& cmd);

    // ========================================================================
    // Special Command Handlers
    // ========================================================================

    /**
     * Print help message with all available commands
     */
    static void printHelp();

    /**
     * Print system status (heap, state, radios)
     */
    static void printStatus();

    /**
     * Print menu items with numbers
     */
    static void printMenu();

    // ========================================================================
    // State Variables
    // ========================================================================

    static char lineBuffer[BUF_LENGTH];    // Command line buffer
    static uint8_t bufferIndex;            // Current position in buffer
    static bool commandReady;              // True when command parsed and ready
    static MenuItem pendingCommand;        // Command waiting to execute
    static bool stopFlag;                  // True when stop requested
    static bool echoEnabled;               // True to echo display to serial
    static bool verboseLogging;            // True for verbose output
    static bool initialized;               // True after init() called
};

#endif // TERMINAL_H
