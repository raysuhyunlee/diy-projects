#pragma once

#include <stdint.h>

#include "state.h"

void scheduler_begin();

// Re-evaluates desired relay states and drives the GPIOs if anything changed.
// time_valid=false suppresses windowed channels (only always_on still runs); this avoids
// random switching while NTP has not synced yet.
void scheduler_tick(const Config& cfg, uint16_t minute_of_day, bool time_valid);

bool scheduler_lights_on();
bool scheduler_pump_on();
bool scheduler_fan_on();
