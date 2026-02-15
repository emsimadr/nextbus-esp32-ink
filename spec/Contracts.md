
# Contracts and compatibility

This repo consumes the NextBus API contract from `nextbus-core`.

## Required fields for /v1/board items
- key
- status
- arrival.minutes when status in {ok, stale}
- as_of (top-level) or per item
- arrival.source (optional for display)

## Compatibility rule
Firmware should remain compatible with any additive changes to v1 responses.

## Suggested practice
- Import pinned contract files from `nextbus-core/spec/contracts` into `spec/contracts/` in this repo.
- Track the pinned version in `docs/CONTRACT_VERSION.md`.
