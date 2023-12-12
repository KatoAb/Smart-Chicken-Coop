#pragma once

#include "wifi.h"
#include "PubSubClient.h"

//Note to self:
//declare variables and instances here and as extern for when they need to be accessed outside of the .cpp-file
//extern meaning that it is decleared here, and defined elsewhere (pref. in assosiated source file).

//variables for storing values subscribed to
extern float setTemperature;
extern float realTemperature;

//Class instances
extern PubSubClient client; //Instance of PubSubClient class

//*****FUNCTIONS*****

void wifiSetup(); //for setup in main

void mqttSetup(); //for setup in main

void callback(char* topic, byte* message, unsigned int length);

void reconnect(); 

void testMqtt();