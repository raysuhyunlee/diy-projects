#pragma once

#include <stddef.h>

#include "state.h"

constexpr size_t HISTORY_LEN = 288;  // 24 h at 5-min interval

void sensors_begin();
bool sensors_read(float& temp_c, float& humidity);

void history_push(const Sample& s);
size_t history_size();
const Sample& history_at(size_t i);  // 0 = oldest
