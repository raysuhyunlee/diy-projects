#pragma once

#include <stdint.h>

struct ChannelConfig {
  uint16_t start_minute;  // 0..1439
  uint16_t end_minute;    // 0..1439
  bool always_on;
};

struct Config {
  ChannelConfig lights;
  ChannelConfig pump;
  ChannelConfig fan;
};

struct Sample {
  uint32_t epoch;
  float temp_c;
  float humidity;
};
