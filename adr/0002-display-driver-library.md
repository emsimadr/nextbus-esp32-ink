
# ADR 0002: Choose an e-ink driver library

## Context

E-ink displays vary by controller and need a reliable driver supporting partial refresh if possible.

The confirmed hardware is:
- **Display:** GDEY0213B74 2.13" black/white (122x250 pixels, SSD1680 controller)
- **Driver board:** Seeed ePaper Driver Board V2
- **MCU:** Seeed XIAO ESP32-C3 (RISC-V, single-core, SPI interface)

## Options

1. **GxEPD2** (common in Arduino/PlatformIO) -- mature, supports the SSD1680 controller, includes partial refresh. Depends on Adafruit GFX for drawing primitives.
2. **Seeed GFX** (vendor library) -- fork of TFT_eSPI with ePaper extensions. Online configurator for pin/display combo.
3. **Vendor raw driver** (GDEY0213B74_Arduino sample) -- direct SPI, no graphics primitives. Maximum control but no text/font support.
4. **LVGL + driver bindings** -- full UI framework with display abstraction. Heavy for a 2-row text display.

## Decision

**GxEPD2** (`zinggjm/GxEPD2@^1`) with the **exact panel class `GxEPD2_213_GDEY0213B74`**.

- Display class: `GxEPD2_213_GDEY0213B74` (GDEY0213B74 122x250, SSD1680)
- Landscape: `display.setRotation(1)` -- rotation handled by SSD1680 hardware
- Drawing: Adafruit GFX primitives (`drawRect`, `fillRect`, `setCursor`, `print`, `getTextBounds`)
- Fonts: built-in GFX fonts (`FreeSans9pt7b`, `FreeSansBold9pt7b`) for MVP
- Update: paged drawing via `firstPage()` / `nextPage()`
- Partial refresh: supported via `display.display(true)`

### History

- Initially chose `GxEPD2_213_BN` (wrong panel variant) -- display was blank.
- Switched to Seeed GFX (ADR 0005) -- rendered but with severe compression/wrap artifacts due to broken rotation in EPaper::update().
- Returned to GxEPD2 with the correct `GxEPD2_213_GDEY0213B74` class. Hardware rotation works natively.

## SPI Pin Mapping (Seeed XIAO ESP32-C3 + ePaper Driver Board V2)

| Signal | XIAO Label | GPIO |
|--------|------------|------|
| SCK    | D8         | 8    |
| MISO   | D9         | 9    |
| MOSI   | D10        | 10   |
| CS     | D1         | 3    |
| DC     | D3         | 5    |
| RST    | D0         | 2    |
| BUSY   | D2         | 4    |

**Critical:** On ESP32-C3, `SPI.begin(SCK, MISO, MOSI, CS)` must be called
explicitly before `display.init()` because the default SPI pins do not match
the ePaper Driver Board V2 wiring.

Pin mapping defined in `include/config.h`.

## Consequences

- GxEPD2 pulls in Adafruit GFX and Adafruit BusIO as transitive dependencies (~62% flash usage total for the full firmware).
- Partial refresh keeps display updates fast (~0.3s) for the 60s poll cycle.
- Full refresh every 20 cycles prevents ghosting.
- `setRotation(1)` gives native landscape (250x122) with no software rotation overhead.
- Custom fonts or icons can be added later without changing the driver.
