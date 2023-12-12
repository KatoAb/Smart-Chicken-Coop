#include "rotaryEncoder.h"
#include "Arduino.h"
#include "variables.h"

//Instance of the rotary encoder lib class
AiEsp32RotaryEncoder rotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

//paramaters for button
unsigned long shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned long longPressAfterMiliseconds = 500;  //how long long press shoud be.

//Flags for the button functionalities
bool shortButtonActive = false; //Initialize flags
bool longButtonActive = false; //Initialize flags

float readValue = 0; // readValue variable for 


//Short and long button click functions
void onButtonShortClick() {
  Serial.print("button SHORT press ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");

  shortButtonActive = true; //Flag for short button press

}

void onButtonLongClick() {
  Serial.print("button LONG press ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
  
  longButtonActive = true; //Flag for long button press
}

//Function for reading button press
void handleRotaryButton() {
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;

  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();

  if (isEncoderButtonDown) {
    Serial.print("+");  //Debug for serial display when holding button
    if (!wasButtonDown) {
      //start measuring
      lastTimeButtonDown = millis();
    }
    //else wait since button is still down
    wasButtonDown = true;
    return;
  }

  //button is up
  if (wasButtonDown) {
    Serial.println("");
    //Check if long or short click
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      onButtonLongClick();
    } else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
      onButtonShortClick();
    }
  }
  wasButtonDown = false;
}
//********** button handling ----
//********** button handling ----
//********** button handling ----
//********** button handling ----

//Long press for accessing the adjust value, turn knob to adjust value, short click to confirm and set the value,

void rotaryLoop() {
  static bool adjusted = false; //

  //Handle button presses
  handleRotaryButton();

  //Check for long button press to adjust temperature
  if (longButtonActive) {
    if (rotaryEncoder.encoderChanged()) {
      // Update the readValue based on the encoder rotation
      readValue = static_cast<float>(rotaryEncoder.readEncoder())/2; //casting to float and dividing to have 0.5 increments
      Serial.print("Temp Value Adjusted: "); //Debug
      Serial.println(readValue); //Debug
      adjusted = true; //Set flag for value adjusted,
    }
  }

  //Check for short button press to confirm and update setTemperature variable,
  //The adjusted flag is for making sure that a value is set before confirming
  if (shortButtonActive && adjusted) { //Checking the flags
    setTemperature = readValue; // Update setTemperature with adjusted value
    Serial.print("Set Temperature: "); //Debug
    Serial.println(setTemperature); //Debug

    // Reset flags after updating setTemperature
    longButtonActive = false;
    adjusted = false;
    shortButtonActive = false;
  }
}

//using internal RAM for this to ensure that the encoder works as good as possible
//using IRAM will hopefully make it less annoying to use
void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

//Setup function, boundries, acceleration(turned off, as value range is short)
void rotarySetup() {

  //initialize rotary encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  //set boundaries and set value cycling to false, not cycling
  rotaryEncoder.setBoundaries(30, 60, false); //circleValues meaning when max go to min and vice versa
  //Don't want acceleration as it is just annoying for this short value range
  rotaryEncoder.setAcceleration(0);  //Larger number = more accelearation; 0 means disabled acceleration
}
