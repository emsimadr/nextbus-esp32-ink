// nextbus-esp32-ink -- main entry point
// See: plan/Iteration-01.md for scope
// See: spec/FirmwareSpec.md for behavior

#ifndef UNIT_TEST  // Exclude from native test builds

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "model.h"
#include "wifi_manager.h"
#include "api_client.h"
#include "renderer.h"

// --- State ---
static char responseBuf[2048];
static BoardData boardData;
static int consecutiveFails = 0;
static int partialRefreshCount = 0;
static bool hasDisplayedData = false;

// --- Helpers ---

static unsigned long getPollInterval() {
    if (consecutiveFails >= MAX_CONSECUTIVE_FAILS) {
        return BACKOFF_INTERVAL_MS;
    }
    return POLL_INTERVAL_MS;
}

static bool needsFullRefresh() {
    return (partialRefreshCount >= FULL_REFRESH_EVERY) || !hasDisplayedData;
}

// --- Arduino entry points ---

void setup() {
    Serial.begin(115200);
    // ESP32-C3 USB-Serial/JTAG needs time to enumerate after reset.
    // Without this delay, early Serial output is lost.
    delay(3000);

#ifdef DEBUG
    Serial.println();
    Serial.println("=== nextbus-esp32-ink ===");
    Serial.println("Starting up...");
    Serial.flush();
#endif

    // Initialize display first (shows boot message)
    displaySetup();
    displayShowMessage("Connecting...");

    // Initialize WiFi
    wifiSetup();

    // Wait for initial WiFi connection (up to 10 seconds)
    unsigned long start = millis();
    while (!wifiIsConnected() && (millis() - start < 10000)) {
        delay(500);
#ifdef DEBUG
        Serial.print(".");
#endif
    }

#ifdef DEBUG
    Serial.println();
    if (wifiIsConnected()) {
        Serial.print("[Main] WiFi connected. IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("[Main] WiFi not yet connected, will retry in loop.");
    }
#endif
}

void loop() {
    // 1. Ensure WiFi is connected
    if (!wifiEnsureConnected()) {
        // Not connected yet -- show WiFi error on display (but only update once)
        if (hasDisplayedData) {
            // Keep showing last data, just update WiFi indicator on next render
        } else {
            displayShowMessage("WiFi connecting...");
        }
        delay(1000);  // Check again in 1 second
        return;
    }

    // 2. Fetch board data from API
    int httpCode = fetchBoard(API_BASE_URL, API_KEY, responseBuf, sizeof(responseBuf));

#ifdef DEBUG
    Serial.print("[Main] HTTP ");
    Serial.print(httpCode);
    Serial.print(" | Response length: ");
    Serial.println(strlen(responseBuf));
#endif

    if (httpCode == 200) {
        // 3. Parse the response
        BoardData newData;
        if (parseBoardResponse(responseBuf, newData)) {
            boardData = newData;
            consecutiveFails = 0;

#ifdef DEBUG
            Serial.print("[Main] Parsed ");
            Serial.print(boardData.item_count);
            Serial.print(" items. as_of=");
            Serial.println(boardData.as_of);
            for (int i = 0; i < boardData.item_count; i++) {
                Serial.print("  [");
                Serial.print(i);
                Serial.print("] ");
                Serial.print(boardData.items[i].label);
                Serial.print(" status=");
                Serial.print((int)boardData.items[i].status);
                if (boardData.items[i].arrival.valid) {
                    Serial.print(" min=");
                    Serial.print(boardData.items[i].arrival.minutes);
                    Serial.print(" leave=");
                    Serial.print(boardData.items[i].arrival.leave_in_minutes);
                }
                Serial.println();
            }
#endif
        } else {
            // Parse error -- treat as failure
#ifdef DEBUG
            Serial.println("[Main] JSON parse error");
#endif
            consecutiveFails++;
        }
    } else {
        // HTTP error or network error
        consecutiveFails++;
#ifdef DEBUG
        Serial.print("[Main] Fetch failed. Consecutive failures: ");
        Serial.println(consecutiveFails);
#endif
    }

    // 4. Render to display
    bool fullRefresh = needsFullRefresh();
    displayRender(boardData, fullRefresh, wifiIsConnected());
    hasDisplayedData = true;

    if (fullRefresh) {
        partialRefreshCount = 0;
    } else {
        partialRefreshCount++;
    }

#ifdef DEBUG
    Serial.print("[Main] Rendered (");
    Serial.print(fullRefresh ? "full" : "partial");
    Serial.print("). Next poll in ");
    Serial.print(getPollInterval() / 1000);
    Serial.println("s");
#endif

    // 5. Wait for next poll
    delay(getPollInterval());
}

#endif // UNIT_TEST
