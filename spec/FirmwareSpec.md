
# Firmware spec

## Networking
- Connect to WiFi on boot.
- If WiFi fails, retry with exponential backoff:
  - 5s, 15s, 30s, 60s, then every 5 minutes
- When WiFi reconnects, resume normal polling cadence.

## Polling
- Poll interval: 60 seconds (nominal)
- On API error:
  - if response includes `status: stale`, treat as success but mark stale
  - if no usable data, keep last screen content and show error marker
- Backoff on repeated API failures (independent of WiFi backoff):
  - After 3 consecutive API failures, poll every 2 minutes until success.

## Time handling
Preferred: use server `as_of` and arrival timestamps; device can avoid NTP for MVP.
- Display "updated" using `as_of` if possible, else show a simple dot indicator for freshness.

## Data source
Preferred endpoint:
- `GET /v1/board`

Alternate (only if needed):
- multiple `GET /v1/next_arrival` calls

## JSON parsing
- Must handle missing fields gracefully.
- Treat unknown `status` as error.

## Configuration
MVP configuration is compile-time constants:
- API_BASE_URL (including host and port)
- API_KEY (optional header)
- MAX_WINDOW_MINUTES (default 30)
- LEAVE_THRESHOLD_MINUTES (default 6)
- BOARD_KEYS or index mapping for display rows

Later configuration options (not MVP):
- captive portal for WiFi + API settings
- OTA config updates

## Power
MVP (Iteration 1) assumes wall power.

Iteration 2 adds arrival-driven adaptive deep sleep (see ADR-0004 and `plan/Iteration-02.md`):
- Sleep duration scales with `arrival.minutes`: longer sleep when buses are far, 60s sleep when close.
- Target: 4 weeks minimum on a 3000mAh cell.
- WiFi reconnect after deep-sleep wake adds 1-3s per cycle.

## Logging
- Minimal serial logs behind a compile-time flag.
- Do not print secrets.
