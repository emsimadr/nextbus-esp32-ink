#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// Compile-time configuration for nextbus-esp32-ink
// See: spec/FirmwareSpec.md > Configuration
// ============================================================

// --- Secrets (WiFi + API credentials) ---
// Copy include/secrets.h.example to include/secrets.h and fill in your values.
// secrets.h is gitignored -- never commit it.
#include "secrets.h"

// --- Display thresholds (see spec/UX.md > Visualization) ---
#define MAX_WINDOW_MINUTES      30
#define LEAVE_THRESHOLD_MINUTES 6

// --- Polling (see spec/FirmwareSpec.md > Polling) ---
#define POLL_INTERVAL_MS        60000   // 60 seconds
#define BACKOFF_INTERVAL_MS     120000  // 2 minutes after 3 failures
#define MAX_CONSECUTIVE_FAILS   3

// --- Display refresh (see spec/UX.md > Refresh policy) ---
#define FULL_REFRESH_EVERY      20      // full refresh every N partial updates

// --- E-ink SPI pin mapping for Seeed XIAO ESP32-C3 ---
// Matches the Seeed ePaper Driver Board V2 pinout.
//   ePaper Pin  XIAO Label  GPIO
//   CS          D1          3
//   DC          D3          5
//   RST         D0          2
//   BUSY        D2          4
//   SCK         D8          8    (default SPI)
//   MOSI        D10         10   (default SPI)
#define EINK_CS     3
#define EINK_DC     5
#define EINK_RST    2
#define EINK_BUSY   4
#define EINK_SCK    8
#define EINK_MISO   9
#define EINK_MOSI   10

#endif // CONFIG_H
