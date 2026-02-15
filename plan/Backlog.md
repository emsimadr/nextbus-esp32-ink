
# Backlog

## Iteration 1 -- MVP (wall power, 2-row display) -- COMPLETE

- ESP32 polls /v1/board every 60s
- Parses response and renders 2-row display
- Visualization bar + leave threshold
- Handles stale/no_service/error states
- Basic WiFi reconnect and API backoff
- Periodic full refresh to mitigate ghosting
- mDNS resolution for `.local` API hostnames

See: `plan/Iteration-01.md`

### Known issues / tech debt from Iteration 1
- Display library: went through GxEPD2 (wrong class) -> Seeed GFX (broken rotation) -> GxEPD2 (correct class). See ADR 0002 and ADR 0005.
- Native unit tests require gcc/g++ on PATH (not available on Windows without MinGW/MSYS2). Tests pass on Linux/macOS.
- USB CDC serial on XIAO ESP32-C3 needs 3s startup delay to capture early boot messages.
- Partial refresh (`display(true)`) not yet exercised on hardware -- currently using full refresh for every update.

### UI polish (low priority, do after hardware validation)
- **Local time display:** Header timestamp currently shows UTC from server `as_of`. Convert to local time (options: timezone offset from API, NTP, or compile-time offset).
- **Bus icons on timeline:** Add small bus/dot icons on the approach bar to visualize the next bus and following buses (alternatives) at their relative positions.
- **Condense layout:** Reduce unnecessary whitespace between header, rows, and footer to make better use of the 122px vertical space.

## Iteration 2 -- Battery power and enhancements

- Arrival-driven adaptive deep sleep (ADR-0004)
- Button/touch wake override (optional GPIO)
- OTA firmware updates with safe rollback
- More sophisticated graphics/icons
- Support more than 2 lines or paginated view
- Switch from full refresh to partial refresh for normal updates

See: `plan/Iteration-02.md`

## Iteration 3 -- Configuration and discovery

- Captive portal for WiFi + API settings (no recompile)
- GPS-supported map view for selecting bus lines and stops

See: `plan/Iteration-03.md`

## Future (unscheduled)

- NTP / RTC for local time display
- Multi-device or multi-home management
- Cloud sync for configuration
- Second-by-second countdown mode
