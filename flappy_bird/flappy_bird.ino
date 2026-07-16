#include <Servo.h>

Servo wing;
int wing_pos = 1500;

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  Serial.begin(9600);
  wing.attach(2);
  wing.writeMicroseconds(wing_pos);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(A0);

  if (val > 1) {
    int flap_delay = (1023 - val) / 3 + 100;
    if (wing_pos == 1700) {
      wing_pos = 1300;
    } else {
      wing_pos = 1700;
    }
    wing.writeMicroseconds(wing_pos);
    Serial.println(wing_pos, DEC);
    delay(flap_delay);
  } else {
    delay(10);
  }
}
