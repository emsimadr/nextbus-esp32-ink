
# Iteration 03 plan: Configuration and discovery

Prerequisite: Iteration 02 complete (battery-powered, multi-route).

## Goal

Remove the need for compile-time configuration. Users can set up WiFi, API connection, and select bus routes/stops directly from the device.

## In scope

### Captive portal configuration
- On first boot (or config reset), device hosts a WiFi access point with a captive portal
- Web UI to configure:
  - WiFi SSID and password
  - API base URL and API key
  - Display preferences (MAX_WINDOW_MINUTES, LEAVE_THRESHOLD_MINUTES)
- Settings persisted to NVS (non-volatile storage)

### GPS-supported map view for stop selection
- Captive portal includes a map-based UI for discovering bus lines and stops
- Uses device GPS or browser geolocation to center the map on the user's location
- User selects stops visually instead of entering route/stop IDs manually
- Requires integration with MBTA route/stop discovery (may need a new API endpoint or GTFS data)

## Out of scope
- OTA config updates without captive portal (covered if OTA is in Iteration 2)
- Multi-device management
- Cloud sync

## Acceptance criteria
- Fresh device can be fully configured without recompiling firmware.
- WiFi credentials and API settings persist across reboots.
- Map view shows nearby bus stops based on location.
- Selected stops appear on the e-ink display after configuration completes.
- Existing Iteration 01 and 02 acceptance criteria still pass.
