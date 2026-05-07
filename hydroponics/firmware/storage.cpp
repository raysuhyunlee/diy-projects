#include "storage.h"

#include <Preferences.h>

namespace {
Preferences prefs;
constexpr const char* NS = "hydro";

// Schedule defaults. Minute-of-day in [0, 1440). 06:00 = 360, 22:00 = 1320.
// If start == end, the channel is off (unless always_on). If end < start, the window wraps.
constexpr uint16_t DEF_LIGHTS_START = 6 * 60;
constexpr uint16_t DEF_LIGHTS_END = 22 * 60;
constexpr bool DEF_LIGHTS_ALWAYS = false;
constexpr uint16_t DEF_PUMP_START = 0;
constexpr uint16_t DEF_PUMP_END = 0;
constexpr bool DEF_PUMP_ALWAYS = true;
constexpr uint16_t DEF_FAN_START = 0;
constexpr uint16_t DEF_FAN_END = 0;
constexpr bool DEF_FAN_ALWAYS = false;

ChannelConfig load_channel(const char* prefix, uint16_t def_start, uint16_t def_end,
                           bool def_always) {
  char key[8];
  ChannelConfig c;
  snprintf(key, sizeof(key), "%ss", prefix);
  c.start_minute = prefs.getUShort(key, def_start);
  snprintf(key, sizeof(key), "%se", prefix);
  c.end_minute = prefs.getUShort(key, def_end);
  snprintf(key, sizeof(key), "%sa", prefix);
  c.always_on = prefs.getBool(key, def_always);
  return c;
}

void save_channel(const char* prefix, const ChannelConfig& c) {
  char key[8];
  snprintf(key, sizeof(key), "%ss", prefix);
  prefs.putUShort(key, c.start_minute);
  snprintf(key, sizeof(key), "%se", prefix);
  prefs.putUShort(key, c.end_minute);
  snprintf(key, sizeof(key), "%sa", prefix);
  prefs.putBool(key, c.always_on);
}
}  // namespace

void storage_begin() { prefs.begin(NS, false); }

Config storage_load() {
  Config c;
  c.lights = load_channel("l", DEF_LIGHTS_START, DEF_LIGHTS_END, DEF_LIGHTS_ALWAYS);
  c.pump = load_channel("p", DEF_PUMP_START, DEF_PUMP_END, DEF_PUMP_ALWAYS);
  c.fan = load_channel("f", DEF_FAN_START, DEF_FAN_END, DEF_FAN_ALWAYS);
  return c;
}

void storage_save(const Config& c) {
  save_channel("l", c.lights);
  save_channel("p", c.pump);
  save_channel("f", c.fan);
}
