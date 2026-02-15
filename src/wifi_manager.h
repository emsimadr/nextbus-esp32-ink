#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// WiFi connection manager with exponential backoff.
// See: spec/FirmwareSpec.md > Networking

// Initialize WiFi and begin first connection attempt.
void wifiSetup();

// Ensure WiFi is connected. If disconnected, attempts reconnect
// with backoff: 5s, 15s, 30s, 60s, then every 5 minutes.
// Returns true if connected, false if still reconnecting.
bool wifiEnsureConnected();

// Check if WiFi is currently connected.
bool wifiIsConnected();

#endif // WIFI_MANAGER_H
