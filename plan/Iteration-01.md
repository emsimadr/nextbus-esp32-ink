
# Iteration 01 plan: Display the board (2 lines)

## Goal
A working firmware app that:
- connects to WiFi
- calls `/v1/board` once per minute
- displays up to 2 board items
- shows a leave threshold visualization

## Status

**Core functionality: COMPLETE.** WiFi, API polling, JSON parsing, and display
rendering are all working. Remaining items are hardware validation of display
output after the GxEPD2 library switch (see below).

### Completed
- PlatformIO project skeleton with esp32c3 and native environments
- WiFi connection with exponential backoff (5s, 15s, 30s, 60s, 5min)
- mDNS client for `.local` hostname resolution (ESPmDNS)
- HTTP GET with optional X-API-Key header and mDNS URL resolution
- JSON parsing for `/v1/board` response (ok, stale, no_service, error)
- Display renderer with landscape layout (header, 2 rows, footer, approach bars)
- Poll loop with 60s interval and 2min backoff after 3 failures
- Full refresh every 20 partial updates
- Unit tests for parsing and helper functions (native build)

### Display driver history
1. GxEPD2 with `GxEPD2_213_BN` -- blank display (wrong panel variant)
2. Seeed GFX with Setup508 -- rendered but compressed/garbled (broken rotation)
3. GxEPD2 with `GxEPD2_213_GDEY0213B74` -- correct panel class, hardware rotation

### Hardware validation -- PASSED
- Display renders correctly in landscape (250x122) with GxEPD2 + GDEY0213B74
- WiFi, mDNS, API polling, JSON parsing, and rendering all confirmed working on device

## In scope
- PlatformIO project skeleton (if using PlatformIO)
- HTTP GET with optional X-API-Key header
- JSON parsing for the /v1/board response
- Render:
  - route or key label
  - minutes
  - status marker (ok, stale, no_service, error)
  - approach bar + threshold tick
- Poll loop with 60s interval
- Simple backoff on failures
- Full refresh every N updates

## Out of scope
- Captive portal config
- OTA
- Deep sleep
- Pagination

## Acceptance criteria
- With a fixture response (local mock or captured JSON), display shows the two rows correctly.
- Device polls every 60s when successful.
- When status is stale, display indicates stale.
- When no_service, display shows no service without crashing.
- When API is unreachable, display shows error marker and retries with backoff.
- Code is organized into small modules: wifi, api_client, model, renderer, main loop.
- Basic unit-testable parsing function exists (even if tests run on host build).
