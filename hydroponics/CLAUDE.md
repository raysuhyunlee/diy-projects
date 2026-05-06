# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

IoT Deep Flow Technique (DFT) hydroponics system. ESP32-CAM firmware controls timed lighting + O2 pump, monitors temp/humidity (DHT22), reads ambient light to auto-off lights for power saving, and serves a web UI for schedule config + sensor graphs. Hardware: ESP32-CAM, DHT22, 4-channel relay, 5V supply. README is in Korean; treat it as the spec source.

## Toolchain

- Arduino IDE / `arduino-cli`. No PlatformIO, no ESP-IDF.
- Board FQBN: `esp32:esp32:esp32cam`.
- Required libs: ESP32 board package (Espressif), DHT sensor library (Adafruit) + Adafruit Unified Sensor.
- Build: `arduino-cli compile --fqbn esp32:esp32:esp32cam firmware/`
- Upload: `arduino-cli upload -p <PORT> --fqbn esp32:esp32:esp32cam firmware/`

## Language

All code, identifiers, comments, and web UI strings in English. README and project-level docs may be Korean.

## Secrets / config

- `config.h` holds WIFI_SSID, WIFI_PASSWORD, TIMEZONE, schedule defaults, etc. — **gitignored, never commit**.
- Commit `config.h.template` with placeholder values so a fresh checkout has a starting point.
- Time is fetched via NTP using a configurable timezone constant.

## Style

- Be terse. Minimal prose, minimal comments — only WHY when non-obvious.
- DRY + SOLID. Prefer the simplest, most direct code that solves the problem.
- No speculative abstractions, no "future-proofing" layers.
- Format with `clang-format` (config at `.clang-format`, Google base, 2-space indent, 100 col).

## ESP32-CAM gotchas

Most GPIOs are consumed by the camera, PSRAM, and flash. Free pins are scarce — document and justify any pin used by DHT22 or the 4ch relay. GPIO4 drives the onboard flash LED (avoid unless intentional). GPIO0 is the boot-mode strap (do not pull at reset). Note pin conflicts with SD card mode (will be used to store the timelapse).
