#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <MFRC522.h>
#include "lcdgraphics.h"
#include "variables.h"
#include "mqtt.h"





Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#include "lcd_code.h"
MFRC522 rfid(SS_PIN, RST_PIN);

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_columns, ROW_NUM, COLUMN_NUM);

#include "readRFID.h"
#include "pageLayout.h"

void setup() {
  Serial.begin(115200);
  SPI.begin(); // init SPI bus

  rfid.PCD_Init(); // init MFRC522
  
  inputString.reserve(32); // Reserve some memory for the input string
  setupDisplay(); // Set up the display

  wifiConnect(); //Connect to wifi

    //Subscribe to topics
  subToClient();
}



void loop() {
mainfunc(); //this runs the main switchcase in pageLayout
}

