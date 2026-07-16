int boostPin = 13;  // temporarily assigned
int latchPin = 6;
int clockPin = 5;
int dataPin[] = {12, 11, 10, 9, 8, 7};

int seconds, minutes, hours;
unsigned long lastTick;


void write(int* dataPin, int clockPin, unsigned char* data);
void runBoost();

void setup() {
  // put your setup code here, to run once:
  for (int i=0; i<6; i++) {
    pinMode(dataPin[i], OUTPUT); 
  }
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(boostPin, OUTPUT);
  Serial.begin(9600);

  seconds = 0;
  minutes = 34;
  hours = 9;
  lastTick = millis();

  unsigned char dat[8*6];
  for (int i=0; i<8*6; i++) {
    dat[i] = 0;
  }
  for (int i=0; i<3; i++) {
    dat[i*16] = 1;
    dat[i*16 + 6] = 1;
  }
  digitalWrite(latchPin, LOW);
  write(dataPin, clockPin, dat);
  digitalWrite(latchPin, HIGH);
}

void loop() {
  runBoost();
  if (millis() - lastTick >= 1000) {
    unsigned char dat[8*6];
    lastTick = millis();

    seconds += 1;
    if (seconds == 60) {
      seconds = 0;
      minutes += 1;
    }
    if (minutes == 60) {
      minutes = 0;
      hours += 1;
    }
    if (hours == 24) {
      hours = 0;
    }


    for(int j=0; j<48; j++) {
      dat[j] = 0;
    }
    digit(hours, dat);
    digit(minutes, &dat[16]);
    digit(seconds, &dat[32]);
    
    digitalWrite(latchPin, LOW);
    write(dataPin, clockPin, dat);
    digitalWrite(latchPin, HIGH);
  }
  /*
  for (int i=0; i<6; i++) {
    unsigned char dat[8*6];
    for(int j=0; j<48; j++) {
      dat[j] = 0;
    }
    
    for(int j=0; j<3; j++) {
      dat[j*16 + i] = 1;
      dat[j*16 + 6 + i] = 1;
    }
    //shiftOut(dataPin, clockPin, MSBFIRST, dat);
    
    digitalWrite(latchPin, LOW);
    write(dataPin, clockPin, dat);
    digitalWrite(latchPin, HIGH);
    
    //Serial.println(dat, HEX);
    delay(500);
  }*/
}

void digit(int number, unsigned char* data)
{
  int tens = number / 10;
  tens = 5 - tens;
  int ones = number % 10;
  ones = 9 - ones;
  data[tens] = 1;
  data[6 + ones] = 1;
}

void write(int* dataPin, int clockPin, unsigned char* data)
{
  digitalWrite(clockPin, LOW);
  for (int i=0; i<8; i++) {
    for (int j=0; j<6; j++) {
      digitalWrite(dataPin[j], data[i + 8*j]);
    }
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(clockPin, LOW);
    delayMicroseconds(1);
  }
}

void runBoost()
{
  static unsigned long lastTick;
  static bool enabled = false;
  
  if (!enabled && micros() - lastTick >= 1) {
    lastTick = micros();
    enabled = true;
    digitalWrite(boostPin, enabled);
  } else if (enabled && micros() - lastTick >= 280) {
    lastTick = micros();
    enabled = false;
    digitalWrite(boostPin, enabled);
  }
}

