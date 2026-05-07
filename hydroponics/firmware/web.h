#pragma once

#include "state.h"

void web_begin(Config* cfg);
void web_tick();
void web_set_sensors(float temp_c, float humidity);
