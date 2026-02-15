
# Testing Strategy (ESP32)

Three test layers, none of which depend on a live MBTA API.

## 1. Unit tests (host-side, native build)

Run on the development machine, no hardware required.

- **JSON parsing:** Feed fixture files from `test/fixtures/` and `examples/` through model functions. Assert that parsed structs contain expected values for all four status shapes (`ok`, `stale`, `no_service`, `error`).
- **Renderer logic (pure math):** Bar fill calculation, threshold hit detection, clamping. These are pure functions with no display dependency.
- **Run with:** `pio test -e native`

## 2. Integration tests (host-side, native build)

Also run on the development machine, but test module boundaries.

- Mock HTTP responses and verify the api_client -> model pipeline produces correct parsed structs.
- Cover all four status shapes plus edge cases: malformed JSON, empty body, unexpected fields, unknown status values.
- **Run with:** `pio test -e native`

## 3. On-device E2E tests

Run on real ESP32 hardware.

- Firmware boots, connects to WiFi, and hits a mock or real API endpoint.
- Verify via serial log markers:
  - WiFi connect/reconnect
  - HTTP response codes
  - Parsed arrival minutes values
  - Status transitions (ok -> stale -> error -> recovery)
  - Backoff timing on repeated failures
  - Display refresh count (partial vs full)
- **Run with:** `pio test -e esp32dev` or manual serial monitor

## General rules

- No test may depend on the live MBTA API.
- Use fixture files under `test/fixtures/` and `examples/` or a local mock server.
- Keep test fixtures in sync with the API v1 contract documented in `.cursor/rules/project.mdc`.
