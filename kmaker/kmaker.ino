#include <Adafruit_NeoPixel.h>

#define SENSOR_NUM 16

char sensorPin[] = {14, 15, 16, 17, 18, 19, 20, 21, 24, 26, 28, 30, 32, 34, 36, 38};

char checkSensor(int);

void setup() {
  // put your setup code here, to run once:
  for(int i=0; i<SENSOR_NUM; i++) {
    pinMode(sensorPin[i], INPUT_PULLUP);
  }
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i<SENSOR_NUM; i++) {
    char isHit;
    isHit = checkSensor(i);
    if (isHit == true) {
      Serial.print(i);
    }
  }
}

char checkSensor(int sensorId) {
  static long timestamp[SENSOR_NUM];

  byte sensorValue = digitalRead(sensorPin[sensorId]);
  if(sensorValue == 0 && timestamp[sensorId] + 500 < millis()) {
    timestamp[sensorId] = millis();
    return true;
  } else {
    return false;
  }
}

