// This program measures the pH level, water temperature, and water level and sends the information to NodeRED

// Millis
unsigned long previousMillis = 0;
const long interval = 5000;  // Interval in milliseconds;

// Constant pins
const int WATER_LEVEL_PIN = 32;
const int PH_PIN = 33;
const int TEMP_PIN = 34;

// Constants found through testing the sensors and datasheets
const int ANALOG_MAX = 4095;
const int WATER_LEVEL_MAX = 1700;
const float PH_OFFSET = 1.5;
const float TEMP_MAX = 50.0;
const float PH_MAX = 14.0;

// Libraries
#include <WiFi.h>
#include <PubSubClient.h>

// Properties of the network
const char* ssid = "SSID";
const char* password = "PW";
const char* mqtt_server = "MQTT_IP";
const int mqttPort = 1883;  

// Defines name of the WiFi client
WiFiClient espWater;
PubSubClient client(espWater);


void setup() {
  // Starts serial communication, sets up Wi-Fi and assigns sensor pins as inputs
  Serial.begin(115200);
  
  setup_wifi();

  pinMode(PH_PIN, INPUT);
  pinMode(WATER_LEVEL_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);
}


void loop() {
  // Check MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  
  unsigned long currentMillis = millis();

  // Perform measurements at a regular interval
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Publish the current pH, water level, and temperature to Node-RED
    measurePH();
    measureTemp();
    measureWaterLevel();
  }
}


// Sensor functions

void measureWaterLevel() {
  // Measure how much drinking water is left
  int waterLevel = analogRead(WATER_LEVEL_PIN);
  waterLevel = map(waterLevel, 0, WATER_LEVEL_MAX, 0, 100);
  waterLevel = constrain(waterLevel, 0, 100); // 0-100% water level

  // Convert water level to string and publish
  char waterLevelString[8];
  dtostrf(waterLevel, 1, 2, waterLevelString);
  Serial.print("Publishing water level in %: ");
  Serial.println(waterLevelString);
  client.publish("WaterLevelIn", waterLevelString);
}

void measurePH() {
  // Measure pH in the drinking water
  float pHValue = analogRead(PH_PIN);
  pHValue = map(pHValue, 0, ANALOG_MAX, ANALOG_MAX, 0); // inverts the analog reading
  pHValue = constrain((PH_OFFSET + (pHValue / ANALOG_MAX) * PH_MAX), 0.0, PH_MAX); // constrains the PH within 0-14

  // Convert pH to string and publish
  char pHValueString[8];
  dtostrf(pHValue, 1, 2, pHValueString);
  Serial.print("Publishing pH level: ");
  Serial.println(pHValueString);
  client.publish("PHIn", pHValueString);
}

void measureTemp() {
  // Measures temperature in the drinking water
  float tempValue = analogRead(TEMP_PIN);
  tempValue = constrain( ((tempValue / ANALOG_MAX) * TEMP_MAX), 0.0, TEMP_MAX); // Temp range from 0 to 50 degrees C

  // Convert temperature to string and publish
  char tempValueString[8];
  dtostrf(tempValue, 1, 2, tempValueString);
  Serial.print("Publishing water temperature: ");
  Serial.println(tempValueString);
  client.publish("WaterTempIn", tempValueString);
}


// Wi-Fi functions

void setup_wifi() {
  // Connect to a WiFi network
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handles topics sent to the sensor node
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
}

void reconnect() {
  // Reconnect to MQTT server
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect("esp32Client")) {
      Serial.println("connected");
    }
    
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    } 
  }
}
