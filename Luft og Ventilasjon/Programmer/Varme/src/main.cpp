#include <Arduino.h>

#include "mqtt.h"


//LED pins
const uint8_t redLED = 4;
const uint8_t blueLED = 2;

//var for storing if LED is off or on
uint8_t ledState = 0;
//var for storing millis
unsigned long previousMillis = 0;


//create a blinking effect with frequency at frequency on LED at LEDpin, 
void blink(uint8_t LEDpin, int freq){
unsigned long currentMillis = millis(); //time now
//checking how long since last digitalWrite
  if (currentMillis - previousMillis >= freq/2) {
    // overwrite previousMillis for next check
    previousMillis = currentMillis;

    // Inverting the LED states for blinking
    if (ledState == LOW){ledState = HIGH;}
    else {ledState = LOW;}

    // Write state to pin specified in parameter, alternating between on and off
    digitalWrite(LEDpin, ledState);
    //Debug
    Serial.print("Freq: ");
    Serial.println(freq);
  }
}

//Function to simulate the heating
void simHeating(){
  uint8_t outLed; //variable for which LED to light
  float deviation = setTemperature - realTemperature;
  int frequency = map(abs(deviation), 0, 15, 6000, 100); //maping the deviation between set and real to correspond to wanted blinking frequency in ms.
  //deciding which color, blue to indicate cooling, red to idicate heating
  if(deviation < 0){ //deviation < 0 means that the actual temp is hotter than wanted temp
    outLed = blueLED;
    digitalWrite(redLED, LOW); //makes sure only one can light at same time
    }
  else{
    outLed = redLED;
    digitalWrite(blueLED, LOW); //makes sure only one can light at same time
    }
  //using the blink function with chosen LED and freq.
  blink(outLed, frequency);

}

void setup() {
  Serial.begin(115200);

  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  wifiSetup();
  mqttSetup();

}

void loop() {
  if(!client.connected()){
    reconnect(); //Connecting to wifi
  }
  client.loop(); //mqtt-client loop

  simHeating();
}
