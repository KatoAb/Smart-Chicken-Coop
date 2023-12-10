//Libraries
#include <Arduino.h>
#include <WiFi.h>
#include <string.h>

//Own project files
#include "variables.h"
#include "oled.h"
#include "rotaryEncoder.h"
#include "bme680.h"
#include "mqtt.h"

//defining variables with a possible value to minimize risk of program acting weird from random var. inits.
float setTemperature = 20;
float prevSetTemp = 19;

//Just for debuging, nothing fancy
void debugValues(){
      Serial.print("setTemperature: ");
      Serial.println(setTemperature);
}

//Publish to mqtt
void uploadMQTT(){
  //publish to respective topics and convert values to c type string
  client.publish("luft/pressure", String(bme.pressure).c_str()); 
  client.publish("luft/humidity", String(bme.humidity).c_str());
  client.publish("luft/gasRes", String(bme.gas_resistance).c_str());
  client.publish("luft/airQual", String(airQualScore()).c_str());
  client.publish("luft/realTemp", String(bme.temperature).c_str());

  if (setTemperature != prevSetTemp){ //Only publish if value is changed
    client.publish("luft/setTemp", String(setTemperature).c_str());
    prevSetTemp = setTemperature;
  }
}

void setup() {
  Serial.begin(115200); //Start serial and set serial speed

  //Initialize and setup the different components
  //these functions are self explainatory
  wifiSetup();
  mqttSetup();
  screenStart();
  bmeSetup();
  rotarySetup();
}

void loop() {
  if(!client.connected()){
    reconnect(); //Connecting to wifi
  }
  client.loop(); //mqtt-client loop

  //Parts running loops
  rotaryLoop(); //Running rotary encoder
  temperatureDisplay(); //Running screen with screen saver

  //Run every 5 seconds, debug and publishing
  static unsigned long lastTime = 0;
  if (millis() - lastTime >= 5000) {
    bmeLoop(); //Running BME-sensor
    debugValues(); //debug stuff
    uploadMQTT(); //Publish function
    lastTime = millis(); // Update last time
  }
}