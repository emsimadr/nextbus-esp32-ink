
# ADR 0003: Use server `as_of` for freshness instead of NTP (MVP)

## Context
NTP adds complexity and more failure modes. The API already returns `as_of`.

## Options
1. Use server `as_of` only (MVP)
2. Add NTP sync
3. Use RTC hardware module

## Decision
Option 1 for MVP.

## Consequences
- Simpler and robust.
- Local time display may be limited unless derived from server timestamps.
- We can add NTP later for nicer UI.
