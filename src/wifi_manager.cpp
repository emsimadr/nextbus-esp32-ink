#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>
#include <ESPmDNS.h>  // Required for .local hostname resolution

// Backoff schedule in milliseconds: 5s, 15s, 30s, 60s, 300s
static const unsigned long BACKOFF_MS[] = {5000, 15000, 30000, 60000, 300000};
static const int BACKOFF_STEPS = sizeof(BACKOFF_MS) / sizeof(BACKOFF_MS[0]);

static int retryIndex = 0;
static unsigned long lastAttemptMs = 0;
static bool connecting = false;

void wifiSetup() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    connecting = true;
    lastAttemptMs = millis();
    retryIndex = 0;

#ifdef DEBUG
    Serial.print("[WiFi] Connecting to ");
    Serial.println(WIFI_SSID);
#endif
}

bool wifiIsConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool wifiEnsureConnected() {
    if (wifiIsConnected()) {
        // Connected -- reset backoff state and init mDNS
        if (connecting) {
            // Start mDNS so .local hostnames (e.g. nextbus-core.local) resolve.
            // See: spec/FirmwareSpec.md > Configuration (API_BASE_URL)
            if (!MDNS.begin("nextbus-display")) {
#ifdef DEBUG
                Serial.println("[WiFi] mDNS init failed");
#endif
            }
#ifdef DEBUG
            Serial.print("[WiFi] Connected. IP: ");
            Serial.println(WiFi.localIP());
            Serial.println("[WiFi] mDNS started (nextbus-display.local)");
#endif
            connecting = false;
        }
        retryIndex = 0;
        return true;
    }

    // Not connected -- attempt reconnect with backoff
    unsigned long now = millis();
    unsigned long backoff = BACKOFF_MS[retryIndex < BACKOFF_STEPS ? retryIndex : BACKOFF_STEPS - 1];

    if (!connecting || (now - lastAttemptMs >= backoff)) {
#ifdef DEBUG
        Serial.print("[WiFi] Reconnecting (attempt ");
        Serial.print(retryIndex + 1);
        Serial.print(", next retry in ");
        Serial.print(backoff / 1000);
        Serial.println("s)...");
#endif
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        lastAttemptMs = now;
        connecting = true;

        if (retryIndex < BACKOFF_STEPS - 1) {
            retryIndex++;
        }
    }

    return false;
}
