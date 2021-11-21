/*
 * NodeMCU V2 - ESP12F
 * Arduino Board Manager :
 *    Add support via adding following line in Preferences.txt
 *    http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *    select
 *      NodeMCU 1.0 (ESP-12E Module) 
 *      
 *  via Arduino Library Manager ADD following LIB's:
 *  
 *  1/ DHT Sensor Lib from Adafruit
 *     https://github.com/adafruit/DHT-sensor-library
 *  2/ InfluxDB Client for Arduino    
 *     https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino
 *      
 *      
 * Reading temperature and humidity
 * using the DHT22 and ESP8266
 * 
 * DHT22 sensor OUTSIDE
 * 
*/

//
// WiFi 
//
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "xxxxxxxxxxxxxxxxxxxxxxxxx"
#define STAPSK  "yyyyyyyyyyyyyyyyyyyyyyyyy"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

//
// DHT22
// https://github.com/adafruit/DHT-sensor-library
//
#include <DHT.h>

#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);

//
// InfluxDB
// Reference:
// https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino/blob/master/examples/BasicWrite/BasicWrite.ino
//
#include <InfluxDbClient.h>

// InfluxDB  server url.
// InfluxDB SERVER : compaq portable
// 
#define INFLUXDB_URL "http://192.168.0.252:8086"

//
// InfluxDB v1 database name 
// influx> create database sensordb
//
#define INFLUXDB_DB_NAME "sensordb"

// InfluxDB client instance for InfluxDB 1
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);

// Data point
// DHT22_01 -> sensor in house
// DHT22_02 -> sensor placed outsite
//
Point sensor("DHT22_02");


//
// SETUP
//

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  // We start by connecting to a WiFi network

  // Wait for serial to initialize.
  while(!Serial) { }
  
  dht.begin();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println();        
  Serial.println();
  Serial.println("Device Started.");
  Serial.println("Running DHT!");
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Check Influxdb server connection
    if (client.validateConnection())
    {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    }
    else
    {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
    }

}

int timeSinceLastRead = 0;

void loop() {

  // Report every 5 seconds.
  if(timeSinceLastRead > 5000) {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) ) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    //Serial.print("H: ");
    //Serial.print(h);
    //Serial.print(" %\t");
    //Serial.print("T: ");
    //Serial.println(t);

    // Store measured value into point
    sensor.clearFields();

    // Data
    sensor.addField("temperature", t);
    sensor.addField("humidity", h);

    Serial.print("Writing: ");
    Serial.println(client.pointToLineProtocol(sensor));

    // Write point
    if (!client.writePoint(sensor))
    {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
    }

    timeSinceLastRead = 0;
  }
  delay(250);
  timeSinceLastRead += 250;
  digitalWrite(LED_BUILTIN, HIGH);
}
