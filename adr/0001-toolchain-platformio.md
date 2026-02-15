
# ADR 0001: Use PlatformIO as the build toolchain (recommended)

## Context
We want reproducible builds, dependency management, and a clean CI path for ESP32 firmware.

## Options
1. PlatformIO
2. Arduino IDE projects
3. ESP-IDF directly

## Decision
Recommend PlatformIO for the initial implementation.

## Consequences
- Easy to manage libraries and builds in CI.
- Developers need PlatformIO installed (VSCode extension or CLI).
- ESP-IDF is still possible later, but migration has a cost.
