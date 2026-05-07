#include "scheduler.h"

#include <Arduino.h>

#include "pins.h"

namespace {
bool lights_on = false;
bool pump_on = false;
bool fan_on = false;

void drive(int pin, bool on) {
#if RELAY_ACTIVE_LOW
  digitalWrite(pin, on ? LOW : HIGH);
#else
  digitalWrite(pin, on ? HIGH : LOW);
#endif
}

bool in_window(uint16_t minute, uint16_t start, uint16_t end) {
  if (start == end) return false;
  if (start < end) return minute >= start && minute < end;
  return minute >= start || minute < end;  // wraps midnight
}

bool channel_on(const ChannelConfig& ch, uint16_t minute, bool time_valid) {
  if (ch.always_on) return true;
  if (!time_valid) return false;
  return in_window(minute, ch.start_minute, ch.end_minute);
}

void apply(int pin, bool desired, bool& current) {
  if (desired != current) {
    drive(pin, desired);
    current = desired;
  }
}
}  // namespace

void scheduler_begin() {
  pinMode(PIN_RELAY_LIGHTS, OUTPUT);
  pinMode(PIN_RELAY_PUMP, OUTPUT);
  pinMode(PIN_RELAY_FAN, OUTPUT);
  drive(PIN_RELAY_LIGHTS, false);
  drive(PIN_RELAY_PUMP, false);
  drive(PIN_RELAY_FAN, false);
}

void scheduler_tick(const Config& cfg, uint16_t minute_of_day, bool time_valid) {
  apply(PIN_RELAY_LIGHTS, channel_on(cfg.lights, minute_of_day, time_valid), lights_on);
  apply(PIN_RELAY_PUMP, channel_on(cfg.pump, minute_of_day, time_valid), pump_on);
  apply(PIN_RELAY_FAN, channel_on(cfg.fan, minute_of_day, time_valid), fan_on);
}

bool scheduler_lights_on() { return lights_on; }
bool scheduler_pump_on() { return pump_on; }
bool scheduler_fan_on() { return fan_on; }
