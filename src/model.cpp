#include "model.h"
#include <ArduinoJson.h>
#include <string.h>

// --- Portable safe string copy (no strlcpy on all platforms) ---
static void safeCopy(char* dst, size_t dstSize, const char* src) {
    if (!src || dstSize == 0) {
        if (dstSize > 0) dst[0] = '\0';
        return;
    }
    strncpy(dst, src, dstSize - 1);
    dst[dstSize - 1] = '\0';
}

// --- Parse status string to enum ---
static Status parseStatus(const char* str) {
    if (!str) return Status::Unknown;
    if (strcmp(str, "ok") == 0) return Status::Ok;
    if (strcmp(str, "stale") == 0) return Status::Stale;
    if (strcmp(str, "no_service") == 0) return Status::NoService;
    if (strcmp(str, "error") == 0) return Status::Error;
    return Status::Unknown;
}

// --- Parse an arrival object ---
static void parseArrival(JsonObject obj, Arrival& arr) {
    arr.valid = !obj.isNull();
    if (!arr.valid) {
        arr.minutes = 0;
        arr.leave_in_minutes = 0;
        arr.source[0] = '\0';
        arr.time_str[0] = '\0';
        arr.trip_id[0] = '\0';
        return;
    }
    arr.minutes = obj["minutes"] | 0;
    arr.leave_in_minutes = obj["leave_in_minutes"] | 0;
    safeCopy(arr.source, sizeof(arr.source), obj["source"] | "");
    safeCopy(arr.time_str, sizeof(arr.time_str), obj["time"] | "");
    safeCopy(arr.trip_id, sizeof(arr.trip_id), obj["trip_id"] | "");
}

// --- Parse a single board item ---
static void parseItemObj(JsonObject obj, BoardItem& item) {
    safeCopy(item.key, sizeof(item.key), obj["key"] | "");
    safeCopy(item.label, sizeof(item.label), obj["label"] | "");
    item.status = parseStatus(obj["status"] | "");
    item.walk_minutes = obj["walk_minutes"] | 0;

    // Arrival (null when no_service or error)
    if (obj["arrival"].isNull()) {
        memset(&item.arrival, 0, sizeof(item.arrival));
        item.arrival.valid = false;
    } else {
        parseArrival(obj["arrival"].as<JsonObject>(), item.arrival);
    }

    // Alternatives (0-2 entries)
    item.alt_count = 0;
    JsonArray alts = obj["alternatives"].as<JsonArray>();
    if (!alts.isNull()) {
        for (JsonObject alt : alts) {
            if (item.alt_count >= 2) break;
            parseArrival(alt, item.alternatives[item.alt_count]);
            item.alt_count++;
        }
    }

    // Stale timestamp (only present when status == stale)
    safeCopy(item.stale_as_of, sizeof(item.stale_as_of), obj["stale_as_of"] | "");

    // Error details (only present when status == error)
    if (!obj["error"].isNull()) {
        JsonObject err = obj["error"].as<JsonObject>();
        safeCopy(item.error_code, sizeof(item.error_code), err["code"] | "");
        safeCopy(item.error_message, sizeof(item.error_message), err["message"] | "");
    } else {
        item.error_code[0] = '\0';
        item.error_message[0] = '\0';
    }
}

// --- Public API ---

bool parseBoardResponse(const char* json, BoardData& out) {
    memset(&out, 0, sizeof(out));

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        out.parse_error = true;
        return false;
    }

    safeCopy(out.as_of, sizeof(out.as_of), doc["as_of"] | "");

    JsonArray items = doc["items"].as<JsonArray>();
    if (items.isNull()) {
        out.parse_error = true;
        return false;
    }

    out.item_count = 0;
    for (JsonObject item : items) {
        if (out.item_count >= 2) break;
        parseItemObj(item, out.items[out.item_count]);
        out.item_count++;
    }

    out.parse_error = false;
    return true;
}

bool parseSingleItem(const char* json, BoardItem& out) {
    memset(&out, 0, sizeof(out));

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        return false;
    }

    parseItemObj(doc.as<JsonObject>(), out);
    return true;
}

float barFill(int minutes, int maxWindow) {
    if (maxWindow <= 0) return 0.0f;
    if (minutes <= 0) return 1.0f;
    if (minutes >= maxWindow) return 0.0f;
    return (float)(maxWindow - minutes) / (float)maxWindow;
}

bool shouldLeave(int minutes, int threshold) {
    return minutes <= threshold;
}
