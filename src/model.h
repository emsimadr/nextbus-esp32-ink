#ifndef MODEL_H
#define MODEL_H

// Data model and JSON parsing for the Bus Tracker API v1.
// Hardware-independent -- testable on native host builds.
// See: .cursor/rules/project.mdc > API Contract

#include <stdint.h>
#include <stdbool.h>

// --- Status enum (matches API status field) ---
enum class Status {
    Ok,
    Stale,
    NoService,
    Error,
    Unknown   // Treat as Error per spec
};

// --- Arrival data ---
struct Arrival {
    int minutes;            // Minutes until bus arrives (>= 0)
    int leave_in_minutes;   // When to leave house (can be negative)
    char source[16];        // "realtime" or "schedule"
    char time_str[32];      // ISO 8601 timestamp
    char trip_id[24];       // MBTA trip ID (may be empty)
    bool valid;             // true if arrival data is present
};

// --- Single board item (one stop/route) ---
struct BoardItem {
    char key[32];
    char label[48];
    Status status;
    Arrival arrival;
    Arrival alternatives[2];
    int alt_count;          // 0-2
    int walk_minutes;
    char stale_as_of[32];   // Only populated when status == Stale
    char error_code[24];
    char error_message[64];
};

// --- Full board response (/v1/board) ---
struct BoardData {
    char as_of[32];         // Top-level server timestamp
    BoardItem items[2];
    int item_count;         // 0-2
    bool parse_error;
};

// Parse a /v1/board multi-stop JSON response.
// Returns true on success, false on parse error.
bool parseBoardResponse(const char* json, BoardData& out);

// Parse a single /v1/board/{key} JSON response.
// Returns true on success, false on parse error.
bool parseSingleItem(const char* json, BoardItem& out);

// Calculate bar fill proportion for the approach visualization.
// Returns 0.0 (empty / far away) to 1.0 (full / arriving now).
// See: spec/UX.md > Visualization
float barFill(int minutes, int maxWindow);

// Check if the user should leave now.
// Returns true when minutes <= threshold.
bool shouldLeave(int minutes, int threshold);

#endif // MODEL_H
