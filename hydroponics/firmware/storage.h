#pragma once

#include "state.h"

void storage_begin();
Config storage_load();
void storage_save(const Config& c);
