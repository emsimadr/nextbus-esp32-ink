
# Contract version pin

This firmware consumes the NextBus Core API v1.

## Core repo

- **Repo:** [emsimadr/nextbus-core](https://github.com/emsimadr/nextbus-core)
- **Branch:** main
- **Commit:** `c76f265` (HEAD as of 2026-02-14)
- **Contract docs:** [api/CONTRACT.md](https://github.com/emsimadr/nextbus-core/blob/main/api/CONTRACT.md)

## Pinned endpoints

| Endpoint            | Used by firmware | Notes                     |
|---------------------|------------------|---------------------------|
| `GET /v1/board`     | Yes              | Primary polling endpoint  |
| `GET /v1/board/{key}` | No (future)   | Single-stop mode          |
| `GET /health`       | No               | Could add connectivity check |

## Local contract fixtures

Sample responses imported from the core repo live in:

- `spec/contracts/ok_realtime.json`
- `spec/contracts/ok_schedule.json`
- `spec/contracts/stale.json`
- `spec/contracts/no_service.json`
- `spec/contracts/error.json`

These are used by the native unit tests (`test/test_native/test_model.cpp`).

## Updating

When the core API changes:

1. Check [api/CONTRACT.md](https://github.com/emsimadr/nextbus-core/blob/main/api/CONTRACT.md) for breaking vs additive changes.
2. Update the commit hash above.
3. Re-export any changed fixture files to `spec/contracts/`.
4. Run `pio test -e native` to verify parsing still passes.
