#pragma once

#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"


#define SEALEVELPRESSURE_HPA (1013.25) //

extern Adafruit_BME680 bme; //Needs to be extern to access it's member variables in main.

// Set up oversampling and filter initialization
void filterSetup();

//Put in setup
void bmeSetup();

//Put in loop
void bmeLoop();

//Serial print the different meassurements, for debugging
void printTemp();
void printPre();
void printHum();
void printGas();

float airQualScore();