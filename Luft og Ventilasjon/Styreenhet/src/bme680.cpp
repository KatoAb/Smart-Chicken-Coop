#include "bme680.h"

#include <Wire.h>

//A lot in this file is debugging and shows only in Serial monitor.

Adafruit_BME680 bme; //bme class instance

//Debug functions
void printTemp(){
  Serial.print(F("Temperature = "));
  Serial.print(bme.temperature);
  Serial.println(F(" *C"));
}

void printPre(){
  Serial.print(F("Pressure = "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));
}

void printHum(){
  Serial.print(F("Humidity = "));
  Serial.print(bme.humidity);
  Serial.println(F(" %"));
}

void printGas(){
  Serial.print(F("Gas = "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));
}

//Calculating percentage score of air quality based on several meassurments (returns 0-100%)
float airQualScore(){
  float hum_score, gas_score;
  float hum_reference = 40;
  float gas_lower_limit = 5000;   // Bad air quality limit
  float gas_upper_limit = 50000;  // Good air quality limit 

  // Contribution from humidity
  if (bme.humidity >= 38 && bme.humidity <= 42)
    hum_score = 0.25*100; // Humidity +/-5% around optimum 
  else
  { //sub-optimal
    if (bme.humidity < 38) 
      hum_score = 0.25/hum_reference*bme.humidity*100;
    else
    {
      hum_score = ((-0.25/(100-hum_reference)*bme.humidity)+0.416666)*100;
    }
  }
  // Contribution from gas particles
  if (bme.gas_resistance > gas_upper_limit) bme.gas_resistance = gas_upper_limit; 
  if (bme.gas_resistance < gas_lower_limit) bme.gas_resistance = gas_lower_limit;
  gas_score = (0.75/(gas_upper_limit-gas_lower_limit)*bme.gas_resistance -(gas_lower_limit*(0.75/(gas_upper_limit-gas_lower_limit))))*100;
  
  //Combining results, to a score from 0-100%, where 100 is best, and returning
  return hum_score + gas_score;
}

// Set up oversampling and filter initialization, standard stuff
void filterSetup(){
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void bmeSetup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("BME680 async test"));

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  filterSetup();
}

void bmeLoop() {
  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  Serial.print(F("Reading started at "));
  Serial.print(millis());
  Serial.print(F(" and will finish at "));
  Serial.println(endTime);

  Serial.println(F("You can do other work during BME680 measurement."));


  // Obtain measurement results from BME680. Note that this operation isn't
  // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  Serial.print(F("Reading completed at "));
  Serial.println(millis());

  printTemp();
  printPre();
  printHum();
  printGas();

  Serial.println();
}