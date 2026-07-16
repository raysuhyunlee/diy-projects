/* 
 *  Centreefugal firmware using ubidots
 *  Created by kokoafab
 *  Modified by Suhyun Lee
 */

// for wifi
#include <SPI.h>
#include <WizFi250.h>

// for json data parsing
#include <ArduinoJson.h>

// for temp sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// for rpm sensor
#include <EnableInterrupt.h>

//Ubidots information
#define TOKEN      "j2RyffL1M5BPrFvP0zRZRmOuWnQgTE"
#define DS_ID      "593e715d762542336e303f01"         // device id
#define VARID_WIND      "593e7f98762542336d1b1bc6"    // wind speed sensor
#define VARID_TEMP      "593e724176254233739d574d"    // temperature sensor
#define VARID_RPM       "593f81607625421b4c385bad"    // rpm sensor

#define VARID_SYSTEM    "593fb3837625422a88f3ddcb"    // system power
#define VARID_MODE      "593f46577625421b4d3307f6"    // mode (pattern/normal)
#define VARID_PATTERN   "593f88b97625421b4c38e7ec"    // pattern (1~5)

//Parse JSON
#define PARSE       "\"value\""
#define ENDPARSE    ","

char ssid[] = "M!";       // wifi SSID (name)
char pass[] = "mfactorial2017";        // wifi password
int status = WL_IDLE_STATUS;       // the Wifi radio's status

/* 
 * Hardware Pin status.
 * Modify this if pins have changed.
 */
#define Pin_WIND      A0      // wind speed sensor
#define ONE_WIRE_BUS  7       // one wire bus required for temperature sensor
#define Pin_RPM       A1      // this pin uses pin change interrupt


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

char server[] = "thing.ubidots.com";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5 * 1000L; // delay between updates, in milliseconds

String rcvbuf;
boolean isConnected = false;

// Initialize the Ethernet client object
WiFiClient client;

void postData(String VARID, float data);
void getData();
void printWifiStatus();
char * floatToString(char * outstr, double val, byte precision, byte widthp);
int getInt(String input);

String valString;
String nameString;

#define RPM_UPDATE_INTERVAL 1000  // update every 1000 ms
#define RPM_PULSES_PER_TURN 12    // index wheel slot number
void count_rpm();   // interrupt function for counting rotation
float get_rpm();   // actual function that calculates rpm
volatile unsigned int rpm_pulses;

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  Serial.println(F("\r\nSerial Init"));

  WiFi.init();

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");

  printWifiStatus();

  // enable pin change interrupt for rpm sensor
  enableInterrupt(Pin_RPM, count_rpm, FALLING);
}

void loop() {
  static boolean readingName = false;  // this indicates if there is any value to read, in the respond from server
  static boolean readingVal = false;  // this indicates if there is any value to read, in the respond from server

  static int system_power;
  static int mode;
  static int pattern;
  static int motor_speed;

  // measure wind speed
  float floatWIND = analogRead(Pin_WIND);
  float voltage = (floatWIND * 5000) / 1024;
  voltage = voltage - 500;
  float wind = voltage / 10;
  
  // measure temperature
  tempSensor.requestTemperatures();
  float temperature = tempSensor.getTempCByIndex(0);

  // measure rpm
  float rpm = get_rpm();

  int i=0;
  while (client.available()) {
    char c = client.read();
    if ( c != NULL ) {
      if (rcvbuf.length() > 50)
        rcvbuf = "";
      rcvbuf += c;
      //Serial.write(c);
    }
    
    // begin to read value name
    if (rcvbuf.endsWith("\"label\": ")) {
      readingName = true;
      nameString = "";
    }

    if (readingName) {
      if (c != ',') {
        nameString += c;
      } else {
        readingName = false;
      }
    }

		// begin to read value
		if (rcvbuf.endsWith("\"value\": ")) {
      readingVal = true;
      valString = "";
    }

    if (readingVal) {
      if (c != '.') {   // read only integer part 
        valString += c;
      }
      else {
        readingVal = false;
        Serial.println("");
        Serial.println(nameString);

        if (nameString.equals(" \"pattern_set\"")) {
          pattern = getInt(valString);
        } else if (nameString.equals(" \"mode_pattern\"")) {
          mode = getInt(valString);
        } else if (nameString.equals(" \"system-on-off\"")) {
          system_power = getInt(valString);
        } else if (nameString.equals(" \"motor_speed\"")) {
          motor_speed = getInt(valString);
        }
      }
    }
  }
  
  if (millis() - lastConnectionTime > postingInterval) {
	  // send sensor data	
    postData(VARID_WIND, wind);
    postData(VARID_TEMP, temperature);
    postData(VARID_RPM, rpm);
    
    getData();

    Serial.print("system: ");
    Serial.print(system_power);
    Serial.print(", ");
    Serial.print("mode: ");
    Serial.print(mode);
    Serial.print(", ");
    Serial.print("pattern: ");
    Serial.print(pattern);
    Serial.print(", ");
    Serial.print("motor speed: ");
    Serial.print(motor_speed);
  }
  rcvbuf = "";

  /*
   * put motor control logic here
   */
  if (system_power == 1) {          // system power ON
    // put system power ON logic here

    if (mode == 0) {  // normal mode
      // put normal mode logic here
    } else if (mode == 1) { // pattern mode
      // put pattern mode logic here
      switch(pattern) {
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
        default:
          break;
      }
    }

  } else if (system_power == 0) {       // system power OFF
    // put system power OFF logic here
  }

}

/*
 * send POST request to ubidots server
 */
void postData(String VARID, float data) {
  uint8_t content_len[6] = {0};
  String TxData;
  char charDATA[20] = "";
  floatToString(charDATA, data, 2, 7 );

  String dataString = "{\"value\": ";
  dataString += charDATA;
  dataString += '}';

  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("send POST");

    // send the HTTP PUT request
    client.print("POST /api/v1.6/variables/");
    client.print(VARID);
    client.println("/values HTTP/1.1");
    client.println("Host: things.ubidots.com");
    client.print("X-Auth-Token: ");
    client.println(TOKEN);
    client.print("Content-Length:");
    client.println(itoa(dataString.length(), (char*)content_len, 10));
    client.println("Content-Type: application/json");
    client.println("Connection: close\r\n");
    client.print(dataString);
    client.println("\r\n");

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}

/*
 * this method makes a HTTP connection to the server
 * and send request to get a data
 */
void getData() {
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");

    // send the HTTP PUT request
    client.print("GET /api/v1.6/datasources/");
    client.print(DS_ID);
    client.print("/variables/");
    client.println(" HTTP/1.1");
    client.println("Host: things.ubidots.com");
    client.print("X-Auth-Token: ");
    client.println(TOKEN);
    client.println("Content-Type: application/json");
    client.println("Connection: close\r\n");

    // note the time that the connection was made
    lastConnectionTime = millis();
    isConnected = true;
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    isConnected = false;
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

char * floatToString(char * outstr, double val, byte precision, byte widthp)
{
  char temp[16];
  byte i;

  // compute the rounding factor and fractional multiplier
  double roundingFactor = 0.5;
  unsigned long mult = 1;
  for (i = 0; i < precision; i++)
  {
    roundingFactor /= 10.0;
    mult *= 10;
  }
  temp[0] = '\0';
  outstr[0] = '\0';

  if (val < 0.0)
  {
    strcpy(outstr, "-\0");
    val = -val;
  }

  val += roundingFactor;

  strcat(outstr, itoa(int(val), temp, 10)); //prints the int part
  if ( precision > 0)
  {
    strcat(outstr, ".\0"); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision - 1;

    while (precision--)
      mult *= 10;

    if (val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val) - val ) * mult;

    unsigned long frac1 = frac;

    while (frac1 /= 10)
      padding--;

    while (padding--)
      strcat(outstr, "0\0");

    strcat(outstr, itoa(frac, temp, 10));
  }

  // generate space padding
  if ((widthp != 0) && (widthp >= strlen(outstr)))
  {
    byte J = 0;
    J = widthp - strlen(outstr);

    for (i = 0; i < J; i++)
    {
      temp[i] = ' ';
    }

    temp[i++] = '\0';
    strcat(temp, outstr);
    strcpy(outstr, temp);
  }
  return outstr;
}

int getInt(String input) {
  int temp;
  char carray[20];
  //Serial.println(input);
  input.toCharArray(carray, sizeof(carray));
  //Serial.println(carray);
  temp = atoi(carray);
  return temp;
}

// interrupt function for counting rotation
void count_rpm() {
  rpm_pulses += 1;
}

float get_rpm() {
  static unsigned long lastUpdate = millis();
  static float rpm;

  if (millis() - lastUpdate >= RPM_UPDATE_INTERVAL) {
    disableInterrupt(Pin_RPM);
    unsigned long interval = millis() - lastUpdate;
    rpm = (60000 / interval) * rpm_pulses / RPM_PULSES_PER_TURN ;
    lastUpdate = millis();
    rpm_pulses = 0;
    enableInterrupt(Pin_RPM, count_rpm, FALLING);
  }

  return rpm;
}
