#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>

#define LIGHT_BULB 16 // Light bulb pin defined to D0
#define WATER_PUMP 5  // Water pump pin define to D1
#define ANALOG_SENSORS A0 // Analog sensors pin defined to A0
#define DHTPIN 4 // DHT sensor pin defined to D2
#define LDR_TRANS 14 // Light sensor transistor pin defined to D5
#define SOIL_SENSOR_TRANS 12 // Soil moisture sensor transistor pin defined to D6
#define DHTTYPE DHT11 // DHT type defined to DHT11

#define FIREBASE_HOST "teste01-7ff81.firebaseio.com"
#define FIREBASE_AUTH "M6vFQcdcoJ0haYB1vnRATlThg41uHzlylc5ugtSX"

#define WIFI_SSID "100%verdao"
#define WIFI_PASSWORD "leteixeira"

int ldrValue; // Variable to store light sensor output value
int rawSoilMoistureValue; // Variable to store soil moisture sensor output value
float DHTHumidityValue; // Variable to store DHT11 sensor humidity output value 
// char DHTData = {}; TODO: Search how to make a object with values

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(LIGHT_BULB, OUTPUT); // Define light bulb pin as output
  pinMode(WATER_PUMP, OUTPUT); // Define water pump pin as output
  pinMode(LDR_TRANS, OUTPUT); // Define light sensor transistor pin as output
  pinMode(SOIL_SENSOR_TRANS, OUTPUT); // Define soil moisture sensor transistor pin as output
  pinMode(ANALOG_SENSORS, INPUT); // Define pin for analog sensors as input

  digitalWrite(LIGHT_BULB, HIGH); // Turn relay pin for light bulb off
  digitalWrite(WATER_PUMP, HIGH); // Turn relay pin for water pump off
  digitalWrite(LDR_TRANS, LOW); // Turn light sensor transistor off
  digitalWrite(SOIL_SENSOR_TRANS, LOW); // Turn light soil moisture sensor transistor off
  digitalWrite(2, LOW); // Turn onboard LED off

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Connect to WiFi Network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500); // Wait for connection
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Connect to Firebase

  dht.begin(); // Start DHT sensor
}

void loop() {
  #define MIN_RH Firebase.getInt("min-rh")
  #define MIN_MOIST Firebase.getInt("min-moist")
  #define MIN_TEMP Firebase.getInt("min-temp")
  #define MAX_TEMP Firebase.getInt("max-temp")
  
  relayControl(readLDR(), readDHTHumidity(), readSoilMoisture(), MIN_RH, MIN_MOIST);
}

int readLDR() {
  digitalWrite(LDR_TRANS, HIGH);
  ldrValue = analogRead(ANALOG_SENSORS);
  digitalWrite(LDR_TRANS, LOW);
  return ldrValue;
}

int readDHTHumidity() {
  return DHTHumidityValue = dht.readHumidity();
}

int readSoilMoisture() {
  digitalWrite(SOIL_SENSOR_TRANS, HIGH);
  rawSoilMoistureValue = analogRead(ANALOG_SENSORS);
  digitalWrite(SOIL_SENSOR_TRANS, LOW);
  int soilMoistureValue = (rawSoilMoistureValue / 1024) * 100; // get actual soil moisture percentage value 
  return soilMoistureValue;
}

void relayControl(int ldrValue, int dhtHumidity, int soilSensorValue, int minRh, int minMoist) {
  luminosityControl(ldrValue);
  humidityControl(dhtHumidity, soilSensorValue, minRh, minMoist);
}

void luminosityControl(int ldrValue) { // void luminosityControl(int ldrValue, int maxTemp) {
  if (ldrValue >= 700) // if (ldrValue >= 700 && DHTTemperature <= maxTemp)
  {
    digitalWrite(LIGHT_BULB, LOW);
  } else {
    digitalWrite(LIGHT_BULB, HIGH);
  }
}

void humidityControl(int dhtHumidity, int soilSensorValue, int minRh, int minMoist) {
  if (dhtHumidity <= minRh && soilSensorValue <= minMoist)
  {
    digitalWrite(WATER_PUMP, LOW);
  } else
  {
    digitalWrite(WATER_PUMP, HIGH);
  }
}
