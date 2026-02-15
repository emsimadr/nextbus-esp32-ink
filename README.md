
# nextbus-esp32-ink specs (starter pack)

This folder is the spec and planning package for building an ESP32 firmware app that polls the NextBus core API and
renders next-arrival information on a minimal e-ink display.

Primary goals:
- Poll the NextBus API once per minute (with sensible backoff on failures)
- Display the next buses for configured lines
- Visualize "approaching" and "leave now" thresholds
- Be reliable (WiFi issues, API issues, stale data)
- Keep firmware simple; all heavy logic stays in the core API

## Expected dependency
This client consumes the NextBus core service contract:
- Preferred endpoint: `GET /v1/board`
- Alternative: `GET /v1/next_arrival` (only if board is not available)

Keep a pinned copy of the contract in this repo under `spec/contracts/` (imported from nextbus-core).
This spec pack does not include the contract itself.

## Getting started

1. **Create your secrets file** (required before building):
   ```
   cp include/secrets.h.example include/secrets.h
   ```
   Edit `include/secrets.h` with your WiFi credentials and API host. This file is gitignored and will not be committed.

2. **Build:** `pio run -e esp32c3`
3. **Test:** `pio test -e native`
4. **Flash:** `pio run -e esp32c3 --target upload`

## Project layout
- `src/` -- firmware source (model, wifi, api client, renderer, main loop)
- `include/` -- compile-time config and secrets
- `spec/` -- product, firmware, UX, and contract specs
- `adr/` -- architecture decision records
- `plan/` -- iteration plans and backlog
- `test/` -- unit tests and test strategy
- `.cursor/rules/` -- Cursor agent rules
