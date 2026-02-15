
# Iteration 02 plan: Battery power and enhancements

Prerequisite: Iteration 01 complete and stable on wall power.

## Goal

A battery-powered device that lasts at least 4 weeks on a single charge, with improved visuals and support for more than 2 routes.

## In scope

### Power management (ADR-0004)
- Arrival-driven adaptive deep sleep between polls
- Sleep duration thresholds: >30min/no_service = 10min, 10-30min = 3min, <10min = 60s, error = 2min
- WiFi reconnect after deep-sleep wake
- Battery target: 4 weeks minimum on 3000mAh cell
- Button/touch wake as optional "wake now" override (single GPIO)

### OTA updates
- Over-the-air firmware update support
- Safe rollback on failed update

### Graphics improvements
- More sophisticated icons and layout
- Better status indicators (replacing text markers with visual elements)

### Multi-route support
- Support more than 2 lines on display
- Paginated or scrollable view if needed

## Out of scope
- Captive portal configuration (Iteration 3)
- GPS map view (Iteration 3)
- NTP / RTC time sync

## Acceptance criteria
- Device runs on battery for 4+ weeks under normal bus-schedule conditions.
- Adaptive polling ramps up as buses approach and ramps down overnight/no-service.
- Button wake (if wired) immediately triggers active-mode polling.
- OTA update succeeds over WiFi without bricking the device.
- Display supports 3+ routes without layout breakage.
- All Iteration 01 acceptance criteria still pass.
