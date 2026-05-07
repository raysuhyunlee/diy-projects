#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <math.h>
#include <time.h>

#include "config.h"
#include "scheduler.h"
#include "sensors.h"
#include "state.h"
#include "storage.h"
#include "web.h"

namespace {
Config config;
unsigned long last_sample_ms = 0;
unsigned long last_history_ms = 0;
constexpr unsigned long SAMPLE_INTERVAL_MS = 5000;
constexpr unsigned long HISTORY_INTERVAL_MS = 5UL * 60UL * 1000UL;
float last_temp = NAN;
float last_humidity = NAN;

void wifi_connect() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 30000) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("ip: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("wifi: connect failed");
  }
}

bool time_valid() { return time(nullptr) > 100000; }

uint16_t minute_of_day() {
  time_t now = time(nullptr);
  struct tm tm;
  localtime_r(&now, &tm);
  return tm.tm_hour * 60 + tm.tm_min;
}
}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("hydroponics boot");

  storage_begin();
  config = storage_load();
  scheduler_begin();
  sensors_begin();

  wifi_connect();
  configTzTime(TIMEZONE, "pool.ntp.org");

  if (MDNS.begin(HOSTNAME)) MDNS.addService("http", "tcp", 80);

  web_begin(&config);
}

void loop() {
  web_tick();

  unsigned long now_ms = millis();
  if (now_ms - last_sample_ms >= SAMPLE_INTERVAL_MS) {
    last_sample_ms = now_ms;
    sensors_read(last_temp, last_humidity);
    web_set_sensors(last_temp, last_humidity);
    scheduler_tick(config, minute_of_day(), time_valid());
  }
  if (now_ms - last_history_ms >= HISTORY_INTERVAL_MS) {
    last_history_ms = now_ms;
    if (time_valid() && !isnan(last_temp)) {
      Sample s;
      s.epoch = (uint32_t)time(nullptr);
      s.temp_c = last_temp;
      s.humidity = last_humidity;
      history_push(s);
    }
  }
}
