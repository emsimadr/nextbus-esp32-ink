
# ADR 0005: Switch from GxEPD2 to Seeed GFX for e-ink driver (SUPERSEDED)

**Status: SUPERSEDED** -- Reverted to GxEPD2. See updated ADR 0002.

## Context

The original firmware (ADR 0002) chose GxEPD2 with `GxEPD2_213_BN` as the e-ink driver.
After flashing to hardware, the display did not render because `GxEPD2_213_BN` was the
wrong panel variant for the GDEY0213B74 display.

Seeed provides a vendor-supported library (Seeed_GFX) with an online configuration tool.
We switched to Seeed GFX to try to resolve the display issue.

## Decision (original)

Switch to Seeed GFX with `BOARD_SCREEN_COMBO 508`.

## Outcome

Seeed GFX rendered content but with severe artifacts:
- Display content appeared compressed/squished (portrait buffer sent to landscape panel).
- `EPaper::update()` does not support `setRotation()` -- rotation must be done in software.
- Multiple software rotation approaches (per-pixel, byte-level, 16bpp sprite) all produced
  wrapping, center gaps, or garbled output due to bit-layout mismatches between the
  TFT_eSprite internal format and the SSD1680 RAM format.

## Resolution

Reverted to GxEPD2 with the **correct panel class `GxEPD2_213_GDEY0213B74`**.
GxEPD2 handles rotation at the SSD1680 command level (`setRotation(1)` configures
data entry mode and address counters) so the framebuffer maps directly to landscape
orientation with zero software overhead.

See ADR 0002 (updated) for the current decision.
