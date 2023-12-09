#pragma once

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_ADDRESS 0x3C // 60, I2C address
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)


//***** FUNCTIONS *****

//Function for drawing text centered(vertically) on screen, default font size 4.
void screenPrintVertCent(String text, int fontSize = 4);
//Draw text 
void screenPrintHorCent(String text, int fontSize, int y);
//Setup function for screen
void screenStart();

void temperatureDisplay();
