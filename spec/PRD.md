
# PRD: nextbus-esp32-ink

## Problem
I want a glanceable, always-available display at home that tells me when the next buses are coming and when I should leave to catch one.

## Users
- Primary: Michael at home, quick glance
- Secondary: household members who don't want to open a phone app

## Goals
- Poll the NextBus API every 60 seconds and display results.
- Show next arrival minutes for multiple configured lines (2 initially).
- Provide a simple visualization of approaching bus with a "leave now" threshold.
- Be robust to WiFi and API outages.
- Minimize screen refresh wear and avoid unnecessary full refreshes.

## Non-goals (for now)
- Editing configuration from the device UI
- Touch input or complex navigation
- Real-time second-by-second countdown
- Multi-home or multiple displays management

## Success criteria
- Display updates at least once per minute when connected.
- Shows "stale" when the API returns stale data.
- Recovers automatically from temporary WiFi or API failures.
- Leaves quickly actionable guidance (leave now vs wait).

## Constraints
- ESP32 resource limits (RAM, flash)
- E-ink refresh time and ghosting
- WiFi reliability
