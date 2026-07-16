#include <ArduinoJson.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "GxEPD2_display_selection_new_style.h"

#define GxEPD2_DRIVER_CLASS GxEPD2_750c

struct WeatherData {
  String location;
  float temperature;
  String condition;
};

WeatherData getWeatherData() {
  WeatherData weatherData;
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(weatherAPIEndpoint);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      weatherData.location = doc["location"]["name"].as<String>();
      weatherData.temperature = doc["current"]["temp_c"].as<float>();
      weatherData.condition = doc["current"]["condition"]["text"].as<String>();
    }
    http.end();
  }
  return weatherData;
}

void displayWeatherData(WeatherData data) {
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(0, 30);
  display.println("Weather Report");
  display.println("Location: " + data.location);
  display.println("Temperature: " + String(data.temperature) + " C");
  display.println("Condition: " + data.condition);
  display.update();
}

void setup() {
  Serial.begin(115200);
  
  /*
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }*/

  // 전자종이 디스플레이를 초기화합니다.
  display.init();
}

void loop() {
  //WeatherData currentWeather = getWeatherData();
  //displayWeatherData(currentWeather);
  //delay(3600000); // 1시간마다 업데이트
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(0, 30);
  display.println("Hello world!");
  display.update();
  delay(10000);
}
