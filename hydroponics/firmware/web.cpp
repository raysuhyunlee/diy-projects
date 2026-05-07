#include "web.h"

#include <Arduino.h>
#include <WebServer.h>
#include <math.h>
#include <time.h>

#include "index_html.h"
#include "scheduler.h"
#include "sensors.h"
#include "storage.h"

namespace {
WebServer server(80);
Config* cfg = nullptr;
float last_temp = NAN;
float last_humidity = NAN;

String f_or_null(float v) {
  if (isnan(v)) return "null";
  return String(v, 2);
}

String channel_json(const ChannelConfig& ch) {
  String s = "{\"start_minute\":";
  s += String(ch.start_minute);
  s += ",\"end_minute\":";
  s += String(ch.end_minute);
  s += ",\"always_on\":";
  s += (ch.always_on ? "true" : "false");
  s += "}";
  return s;
}

void handle_index() { server.send_P(200, "text/html", INDEX_HTML); }

void handle_state() {
  uint32_t epoch = (uint32_t)time(nullptr);
  String s = "{";
  s += "\"temp_c\":" + f_or_null(last_temp);
  s += ",\"humidity\":" + f_or_null(last_humidity);
  s += ",\"lights_on\":" + String(scheduler_lights_on() ? "true" : "false");
  s += ",\"pump_on\":" + String(scheduler_pump_on() ? "true" : "false");
  s += ",\"fan_on\":" + String(scheduler_fan_on() ? "true" : "false");
  s += ",\"epoch\":" + String(epoch > 100000 ? epoch : 0);
  s += ",\"config\":{";
  s += "\"lights\":" + channel_json(cfg->lights);
  s += ",\"pump\":" + channel_json(cfg->pump);
  s += ",\"fan\":" + channel_json(cfg->fan);
  s += "}}";
  server.send(200, "application/json", s);
}

void handle_history() {
  String s = "[";
  size_t n = history_size();
  for (size_t i = 0; i < n; i++) {
    const Sample& x = history_at(i);
    if (i) s += ",";
    s += "{\"t\":" + String(x.epoch);
    s += ",\"tc\":" + String(x.temp_c, 2);
    s += ",\"h\":" + String(x.humidity, 2) + "}";
  }
  s += "]";
  server.send(200, "application/json", s);
}

uint16_t arg_minute(const String& name, uint16_t fallback) {
  if (!server.hasArg(name)) return fallback;
  long v = server.arg(name).toInt();
  if (v < 0) v = 0;
  if (v > 1439) v = 1439;
  return (uint16_t)v;
}

void update_channel(const char* prefix, ChannelConfig& ch) {
  String p(prefix);
  ch.start_minute = arg_minute(p + "_start", ch.start_minute);
  ch.end_minute = arg_minute(p + "_end", ch.end_minute);
  ch.always_on = server.arg(p + "_always") == "1";
}

void handle_schedule_post() {
  update_channel("lights", cfg->lights);
  update_channel("pump", cfg->pump);
  update_channel("fan", cfg->fan);
  storage_save(*cfg);
  server.sendHeader("Location", "/");
  server.send(303);
}
}  // namespace

void web_begin(Config* c) {
  cfg = c;
  server.on("/", HTTP_GET, handle_index);
  server.on("/api/state", HTTP_GET, handle_state);
  server.on("/api/history", HTTP_GET, handle_history);
  server.on("/api/schedule", HTTP_POST, handle_schedule_post);
  server.begin();
}

void web_tick() { server.handleClient(); }

void web_set_sensors(float temp_c, float humidity) {
  last_temp = temp_c;
  last_humidity = humidity;
}
