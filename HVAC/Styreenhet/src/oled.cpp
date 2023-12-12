#include "oled.h"

#include <Wire.h>

#include "bme680.h"
#include "rotaryEncoder.h"

#include "variables.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static long previousMillis = 0;

//Function for drawing text centered(vertically) on screen, default font size 4.
void screenPrintVertCent(String text, int fontSize){
  display.clearDisplay();
  display.setTextSize(fontSize);
  display.setCursor(fontSize, (SCREEN_HEIGHT - (fontSize*6))/2);
  display.println(text);
  display.display();
}

void screenPrintHorCent(String text, int fontSize, int y) {
  //Set font size
  display.setTextSize(fontSize);

  // Get the width of the text to be displayed
  int16_t textWidth = text.length() * (6 * fontSize); // Assuming each character is 6 pixels wide

  // Calculate the x-coordinate to center the text horizontally
  int16_t xPos = (SCREEN_WIDTH - textWidth) / 2;

  display.setCursor(xPos, y);
  display.println(text);
}


void screenStart(){
    // Address 0x3C for 128x64, you might need to change this value (use an I2C scanner)
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  
}

void temperatureDisplay(){
  if(longButtonActive){
    //Descriptive text on top
    display.clearDisplay();
    screenPrintHorCent("Set temp:", 2, 0);

    //Set temperature value
    screenPrintHorCent(String(readValue, 1), 3, 20);

    //Show real temp under
    screenPrintHorCent("Real: "+String(bme.temperature, 1), 2, 48);
  }

  else{
    //Descriptive text on top
    display.clearDisplay();
    screenPrintHorCent("Real temp:", 2, 0);

    //Display real meassured temperature
    screenPrintHorCent(String(bme.temperature, 1), 3, 20); //show on screen with one decimal

    //Show set temp under
    screenPrintHorCent("Set: "+String(setTemperature, 1), 2, 48);
  }

  display.display();
}

