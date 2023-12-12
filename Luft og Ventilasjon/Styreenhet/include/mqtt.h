#pragma once

#include "wifi.h"
#include "PubSubClient.h"


//Class instance
extern PubSubClient client; //Creating an instance of PubSubClient class called client, main needs access.


//*****FUNCTIONS*****
void wifiSetup();

void mqttSetup();

void callback(char* topic, byte* message, unsigned int length);

void reconnect();

void testMqtt();