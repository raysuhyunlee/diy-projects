#include "sensors.h"

#include <DHT.h>
#include <math.h>

#include "pins.h"

namespace {
DHT dht(PIN_DHT22, DHT22);
Sample ring[HISTORY_LEN];
size_t head = 0;
size_t count = 0;
}  // namespace

void sensors_begin() { dht.begin(); }

bool sensors_read(float& temp_c, float& humidity) {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) return false;
  temp_c = t;
  humidity = h;
  return true;
}

void history_push(const Sample& s) {
  ring[head] = s;
  head = (head + 1) % HISTORY_LEN;
  if (count < HISTORY_LEN) count++;
}

size_t history_size() { return count; }

const Sample& history_at(size_t i) {
  size_t start = (count < HISTORY_LEN) ? 0 : head;
  return ring[(start + i) % HISTORY_LEN];
}
