#include <PCF8574.h>
#include <Wire.h>

PCF8574 PCF_01(0x38);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  
  PCF_01.begin();
  Serial.println("Initializing");

  PCF_01.write8(0);

/*
  for(int i=0; i<8; i++) {
    expander.digitalWrite(i, LOW);
  }

  expander.digitalWrite(0, HIGH);*/
}

void loop() {
  // put your main code here, to run repeatedly:
  //for(int i=0; i<10; i++) {
    //for(int j=0; j<10; j++) {
    if(Serial.available() > 0) {
      char a = 0;
      a = Serial.read();
      if (a >= '0' && a <= '9') {
        //PCF_01.write8((a-'0')*16);
        PCF_01.write(4, 0);
        PCF_01.write(5, 0);
        PCF_01.write(6, 0);
        PCF_01.write(7, 1);
      }
    }
      //PCF_01.write8(j);
      //digitalWrite(13, HIGH);
      //delay(500);
      //digitalWrite(13, LOW);
      //delay(500);
    //}
  //}
}
