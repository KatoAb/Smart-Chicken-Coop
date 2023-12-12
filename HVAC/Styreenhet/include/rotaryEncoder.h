#pragma once

#include "AiEsp32RotaryEncoder.h"

#define ROTARY_ENCODER_A_PIN 25
#define ROTARY_ENCODER_B_PIN 26
#define ROTARY_ENCODER_BUTTON_PIN 27
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

//depending on your encoder - try 1,2 or 4 to get expected behaviour
#define ROTARY_ENCODER_STEPS 4 


//Variables to be accesed externally (by oled file)
extern bool longButtonActive; //Initialize flags
extern float readValue; // readValue variable for displaying on OLED

//****** FUNCTIONS ******

//Different handling functions for the button and rotor functionality
void onButtonShortClick();
void onButtonLongClick();
void handleRotaryButton();

//Use in main file
void rotarySetup(); //Put in setup
void rotaryLoop(); //Put in loop
