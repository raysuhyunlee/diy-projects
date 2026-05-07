#pragma once

// ESP32-CAM (AI Thinker) GPIO usage. Pin layout is locked so the future SD slot + camera
// (timelapse) can be added without re-routing.
//
// Header GPIOs and their fate:
//   0   camera XCLK (reserved for future timelapse)
//   1   UART0 TX — kept for serial debug/boot log
//   2   SD_MMC D0 (reserved for future timelapse)
//   3   UART0 RX — REPURPOSED here as relay fan output (serial RX is lost)
//   4   relay lights — note: also drives the on-board flash LED
//   12  relay pump — strap pin (must boot LOW); active-HIGH relay idles LOW, matches
//   13  DHT22 data
//   14  SD_MMC CLK (reserved for future timelapse)
//   15  SD_MMC CMD (reserved for future timelapse)
//   16  PSRAM CS (locked)
//
// Active-HIGH relay assumption: IN idles LOW (module pulldown), drive HIGH to energize.
//
// Caveats baked in:
// - GPIO4 + flash LED: when the lights relay is ON, the flash LED also lights. Cosmetic.
// - GPIO3 as relay fan: UART0 RX is unavailable (TX still prints boot log + Serial.print).
//   During USB flashing, GPIO3 carries the bootloader's incoming flash data, which will
//   chatter the fan relay rapidly for a few seconds. Power the fan from a switch you can
//   toggle during programming, or unplug its load while flashing.

#define PIN_DHT22 13

#define PIN_RELAY_LIGHTS 4
#define PIN_RELAY_PUMP 12
#define PIN_RELAY_FAN 3

// Relay polarity. 0 = active-HIGH (drive HIGH to energize, idle LOW). 1 = active-LOW.
#define RELAY_ACTIVE_LOW 0

// Camera pins (AI Thinker) — reserved for future timelapse, not initialized here.
//   PWDN=32 RESET=-1 XCLK=0 SIOD=26 SIOC=27
//   Y9=35 Y8=34 Y7=39 Y6=36 Y5=21 Y4=19 Y3=18 Y2=5
//   VSYNC=25 HREF=23 PCLK=22
