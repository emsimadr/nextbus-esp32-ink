
# ADR 0004: Arrival-driven adaptive polling for battery power

## Context

The device will run on battery (e.g., 3000mAh 18650 cell). Polling every 60s with WiFi active draws ~160mA average, giving roughly 19 hours of battery life -- not viable.

Most of the day the display does not need frequent updates (overnight, at work, weekends). But when the user is about to leave the house, accurate per-minute data is essential. The poll frequency should match actual need.

## Options

1. **Arrival-driven adaptive polling** -- use `arrival.minutes` from the last API response to decide how long to deep-sleep before the next poll. Buses approach gradually through time bands, so the device naturally ramps up polling as departure gets relevant.

2. **Button/touch wake** -- deep sleep by default, physical button triggers an "active" window of 60s polling for ~30 minutes. Maximum battery savings, but requires user action and extra hardware.

3. **Time-of-day schedule** -- configure fixed active/idle windows (e.g., 7-9am, 5-7pm). Requires NTP or RTC (conflicts with ADR-0003). Rigid, doesn't adapt to schedule changes.

## Decision

Option 1: arrival-driven adaptive polling.

### Sleep duration thresholds

| Last known `arrival.minutes` | Sleep duration |
|------------------------------|---------------|
| > 30 or `no_service`         | 10 minutes    |
| 10 - 30                      | 3 minutes     |
| < 10                         | 60 seconds    |
| `error` / no data            | 2 minutes     |

### Battery target

Minimum 4 weeks on a 3000mAh cell. Back-of-envelope: the device spends most time in the 10-min sleep band (~1mA average), with short bursts of 60s polling when buses are near. Estimated 3-8 weeks depending on bus frequency and time of year.

## Consequences

- No extra hardware required. No NTP dependency. Consistent with ADR-0003.
- If the device is in a 10-min sleep and a bus enters the 30-min window, the first close reading is delayed by up to 10 minutes. Acceptable because buses approach gradually through the time bands.
- WiFi reconnect after deep sleep adds 1-3 seconds per poll cycle. This is accounted for in the power budget.
- Button/touch wake can be layered on later as an optional "wake up NOW" override (see Backlog).
- Implementation is one function that maps `arrival.minutes` to a sleep duration.
- Scheduled for Iteration 2. Iteration 1 assumes wall power.
