#include "api_client.h"
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <string.h>

// Resolve a .local hostname via mDNS and rewrite the URL with the IP.
// ESP32's hostByName() doesn't route .local queries through mDNS,
// so we must resolve manually with MDNS.queryHost().
// Returns true if the URL was resolved (or didn't need resolution).
static bool resolveUrl(const char* inputUrl, char* outputUrl, size_t outLen) {
    // Check if URL contains ".local"
    const char* dotLocal = strstr(inputUrl, ".local");
    if (!dotLocal) {
        strncpy(outputUrl, inputUrl, outLen);
        outputUrl[outLen - 1] = '\0';
        return true;
    }

    // Extract hostname: skip past "http://"
    const char* hostStart = strstr(inputUrl, "://");
    if (!hostStart) return false;
    hostStart += 3;  // skip "://"

    // Hostname is from hostStart to dotLocal (without ".local")
    size_t nameLen = dotLocal - hostStart;
    if (nameLen == 0 || nameLen >= 64) return false;
    char hostname[64];
    memcpy(hostname, hostStart, nameLen);
    hostname[nameLen] = '\0';

    // Resolve via mDNS
    IPAddress ip = MDNS.queryHost(hostname, 3000);  // 3s timeout
    if (ip == IPAddress()) {
#ifdef DEBUG
        Serial.print("[API] mDNS resolve failed for: ");
        Serial.println(hostname);
#endif
        return false;
    }

#ifdef DEBUG
    Serial.print("[API] mDNS resolved ");
    Serial.print(hostname);
    Serial.print(".local -> ");
    Serial.println(ip);
#endif

    // Rebuild URL: scheme + resolved IP + everything after ".local"
    const char* afterLocal = dotLocal + 6;  // skip ".local"
    int schemeLen = hostStart - inputUrl;    // "http://" length
    snprintf(outputUrl, outLen, "%.*s%d.%d.%d.%d%s",
             schemeLen, inputUrl,
             ip[0], ip[1], ip[2], ip[3],
             afterLocal);
    return true;
}

int fetchBoard(const char* baseUrl, const char* apiKey, char* buffer, size_t bufLen) {
    if (bufLen == 0) return -1;
    buffer[0] = '\0';

    // Build URL: baseUrl + "/v1/board", resolving .local hostnames via mDNS
    char rawUrl[256];
    snprintf(rawUrl, sizeof(rawUrl), "%s/v1/board", baseUrl);

    char url[256];
    if (!resolveUrl(rawUrl, url, sizeof(url))) {
        // Fallback: try the original URL (let hostByName handle it)
        strncpy(url, rawUrl, sizeof(url));
        url[sizeof(url) - 1] = '\0';
    }

    WiFiClient client;
    HTTPClient http;

    if (!http.begin(client, url)) {
#ifdef DEBUG
        Serial.println("[API] Failed to begin HTTP connection");
#endif
        return -1;
    }

    // Set auth header if API key is provided
    if (apiKey && apiKey[0] != '\0') {
        http.addHeader("X-API-Key", apiKey);
    }

    http.setTimeout(10000);  // 10 second timeout

    int httpCode = http.GET();

#ifdef DEBUG
    Serial.print("[API] GET ");
    Serial.print(url);
    Serial.print(" -> ");
    Serial.println(httpCode);
#endif

    if (httpCode > 0) {
        // HTTP response received
        String payload = http.getString();
        size_t copyLen = payload.length();
        if (copyLen >= bufLen) {
            copyLen = bufLen - 1;
        }
        memcpy(buffer, payload.c_str(), copyLen);
        buffer[copyLen] = '\0';
    } else {
        // Connection error
#ifdef DEBUG
        Serial.print("[API] Connection error: ");
        Serial.println(http.errorToString(httpCode));
#endif
        http.end();
        return -1;
    }

    http.end();
    return httpCode;
}
