void setupDisplay() {
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(1); // Rotate 90 deg clockwise
  int w = tft.width(); //get height and width from lcd (pixels)
  int h = tft.height();
  Serial.print("width =");
  Serial.println(w);
  Serial.print("height =");
  Serial.println(h);
}

void displaySensorData(){
  
    //Display new value if value is changed
if(airTemp != oldAirTemp){
    tft.fillRect(30, 9, 45, 18, ST77XX_BLACK);
    tft.setCursor(31, 13);
    tft.setTextSize(1);
    tft.setTextColor(defaultTextColor);
    tft.println(airTemp,1);
    oldAirTemp = airTemp;
}

if(airHum != oldAirHum){
    tft.fillRect(30, 50, 45, 18, ST77XX_BLACK);
    tft.setCursor(31, 54);
    tft.setTextSize(1);
    tft.setTextColor(defaultTextColor);
    tft.println(airHum,1);
    oldAirHum = airHum;
}

if(airVOC != oldAirVOC){

    tft.fillRect(30, 94, 45, 18, ST77XX_BLACK);
    tft.setCursor(31, 98);
    tft.setTextSize(1);
    tft.setTextColor(IAQtextColor);
    tft.println(airVOC,1);
    oldAirVOC = airVOC;

}


if(waterTemp != oldWaterTemp){
    tft.fillRect(112, 7, 45, 18, ST77XX_BLACK);
    tft.setCursor(113, 11);
    tft.setTextSize(1);
    tft.setTextColor(defaultTextColor);
    tft.println(waterTemp,1);
    oldWaterTemp = waterTemp;
}

if(PHValue != oldPHValue){
    tft.fillRect(112, 35, 45, 18, ST77XX_BLACK);
    tft.setCursor(113, 39);
    tft.setTextSize(1);
    tft.setTextColor(PHtextColor);
    tft.println(PHValue,1);
    oldPHValue = PHValue;
}

if(foodTime != oldFoodTime){
    tft.fillRect(112, 65, 45, 18, ST77XX_BLACK);
    tft.setCursor(113, 69);
    tft.setTextSize(1);
    tft.setTextColor(defaultTextColor);
    tft.println(foodTime);
    oldFoodTime = foodTime;
}

if(waterLevel != oldWaterLevel){


    tft.fillRect(112, 94, 45, 18, ST77XX_BLACK);
    tft.setCursor(113, 99);
    tft.setTextSize(1);
    tft.setTextColor(waterLevelTextColor);
    tft.println(waterLevel,1);
    oldWaterLevel = waterLevel;
  }




  
}


//Display values when case 3 is entered
void printFirstValues(){
    tft.fillRect(30, 9, 45, 18, ST77XX_BLACK);
    tft.setCursor(31, 13);
    tft.setTextSize(1);
    tft.setTextColor(defaultTextColor);
    tft.println(airTemp,1);
    oldAirTemp = airTemp;


    tft.fillRect(30, 50, 45, 18, ST77XX_BLACK);
    tft.setCursor(31, 54);
    tft.setTextSize(1);
    tft.setTextColor(defaultTextColor);
    tft.println(airHum,1);
    oldAirHum = airHum;



    tft.fillRect(30, 94, 45, 18, ST77XX_BLACK);
    tft.setCursor(31, 98);
    tft.setTextSize(1);
    tft.setTextColor(IAQtextColor);
    tft.println(airVOC,1);
    oldAirVOC = airVOC;



    tft.fillRect(112, 7, 45, 18, ST77XX_BLACK);
    tft.setCursor(113, 11);
    tft.setTextSize(1);
    tft.setTextColor(defaultTextColor);
    tft.println(waterTemp,1);
    oldWaterTemp = waterTemp;


    tft.fillRect(112, 35, 45, 18, ST77XX_BLACK);
    tft.setCursor(113, 39);
    tft.setTextSize(1);
    tft.setTextColor(PHtextColor);
    tft.println(PHValue,1);
    oldPHValue = PHValue;


    tft.fillRect(112, 65, 45, 18, ST77XX_BLACK);
    tft.setCursor(113, 69);
    tft.setTextSize(1);
    tft.setTextColor(defaultTextColor);
    tft.println(foodTime);


    tft.fillRect(112, 94, 45, 18, ST77XX_BLACK);
    tft.setCursor(113, 99);
    tft.setTextSize(1);
    tft.setTextColor(waterLevelTextColor);
    tft.println(waterLevel,1);
    oldWaterLevel = waterLevel;

}
