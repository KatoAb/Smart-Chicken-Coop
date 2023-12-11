


void mainfunc() {

if(millis() - energySaveTimer >= energySaveIntervall){
  tft.fillScreen(ST77XX_BLACK);
  caseState = 12;
}


client.loop();

  if( millis() - casePrintTimer >= printCaseIntervall){ //printing casestate for debugging
    casePrintTimer = millis();
    Serial.println(caseState);
  }

char key = keypad.getKey(); //updating to see if a key has been or is beeing pressed
if(key){
  energySaveTimer = millis();
  Serial.println("energySaveTimer is reset.");
}
switch (caseState) {

  case 0:
  bootTimer = millis();
    tft.fillScreen(ST77XX_BLACK);
    tft.drawBitmap(0, 0, epd_bitmap_chickenpng2, 160, 128, ST77XX_YELLOW);
    caseState = 1;
  break;

  case 1:

  if (millis() - bootTimer >= bootLength){
      tft.fillScreen(ST77XX_BLACK);
      tft.drawBitmap(0, 0, epd_bitmap_homescreen1, 160, 128, ST77XX_WHITE);
      currentHomeScreen = 1;
      picTimer = millis();
      caseState = 2;
  }      
  break;
  
  case 2:
  //Serial.println("case 2");

  if(millis() - picTimer >= homeTime){
       //case to switch between the two homescreens
    switch (currentHomeScreen) {
      case 1:
      tft.fillScreen(ST77XX_BLACK);
      tft.drawBitmap(0, 0, epd_bitmap_homescreen2, 160, 128, ST77XX_WHITE); //change to 2. homescreen
      currentHomeScreen = 2;

      picTimer = millis(); //resetting timer
      break;

      case 2:
      tft.fillScreen(ST77XX_BLACK);
      tft.drawBitmap(0, 0, epd_bitmap_homescreen1, 160, 128, ST77XX_WHITE); //change to 1. homescreen
      currentHomeScreen = 1;
      picTimer = millis(); //resetting timer
      break;
    }

  }
  if (key){
    
    if(key == '*'){ //Switch to sensor value screen
      sensorDataTimeOut = millis();
      tft.fillScreen(ST77XX_BLACK);
      tft.drawBitmap(0, 0, epd_bitmap_sensorValues, 160, 128, ST77XX_WHITE); 

  
      printFirstValues();

      caseState = 3; //jump to case 3 to display sensor values at the correct place
    }

    else if(key >= '0' && key <= '9'){ //Switch to egg statistic screen
    tft.fillScreen(ST77XX_BLACK);
    tft.drawBitmap(0, 0, epd_bitmap_eggData, 160, 128, ST77XX_WHITE); 
    inputString = "";
    timeOutTimer = millis();
    caseState = 4; //jump to state 4 to enter data
    
    }
  }
    if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    energySaveTimer = millis();
    readRFID();
  }

  break;

  case 3:
  //Serial.println("case 3");
displaySensorData();
    if((millis() - sensorDataTimeOut >= sensorTime) || (key == 'B')){ //switch to state 2 if timer runs out or by button press "B"
    caseState = 2;
    }

    if(key == 'A'){
     sensorDataTimeOut = millis() - 10000; //delay timeout by 10 sec 
    }
    if(millis() - sensorDataTimeOut <= sensorTimerWarning){
      //display timeout warning if wanted 
    }
  break;

  case 4:


if (key >= '0' && key <= '9' && inputString.length() < 7) {
    inputString += key;
}
  
  if ((key == '#') && (inputString.length() > 0)) {   
          inputString.remove(inputString.length() - 1);
    }
  

  if(key == 'D' && (inputString.length() > 0)){ //send data to the cloud
    

    sendData();
    tft.fillScreen(ST77XX_BLACK);
    tft.drawBitmap(0, 0, epd_bitmap_datasent, 160, 128, ST77XX_WHITE);
    dataSentTimer = millis();
    caseState = 5;
    
  }

  if (inputString != previousInputString){
    timeOutTimer = millis();
    previousInputString = inputString;
    tft.setTextSize(2);
    tft.fillRect(34, 50, 94, 28, ST77XX_BLACK);
    tft.setCursor(40, 60);
    tft.setTextColor(defaultTextColor);
    tft.println(inputString);
  }

  if(millis() - timeOutTimer >= timeOutTime){
    tft.fillScreen(ST77XX_BLACK);
    tft.drawBitmap(0, 0, epd_bitmap_timedOut, 160, 128, ST77XX_WHITE);
    caseState = 10;
    inputString = "";
    timeOutShowTimer = millis();
  }
  if(key == 'B'){ //go back to homescreen if B is pressed
    caseState = 2;
    inputString = "";
  }
        
  break;

  case 5:
    if(millis() - dataSentTimer >= sentTime){
      inputString = "";
      caseState = 2; //back to homescreen
    }

  break;

  case 6:
  
  rfidAllow = millis();
  if (key == '*'){
    caseState = 7;
    timeOutTimer = millis();
  }
  if(millis() - timeOutTimer >= timeOutTime){ //go to timeout if star has not been pressed after timeOutTime sec
    timeOutShowTimer = millis();

    tft.fillScreen(ST77XX_BLACK);
    tft.drawBitmap(0, 0, epd_bitmap_timedOut, 160, 128, ST77XX_WHITE);
    caseState = 10;
    timeOutShowTimer = millis();
  }
    if(key == 'B'){ //go back to homescreen if B is pressed
    caseState = 2; //back to homescreen
    inputString = "";
  }
  break;

  case 7:
  
  if(key >= '0' && key <= '9' && inputCode.length() < 4){ //(if button is 0<= x <= 9, and length under 4 (gives 4 characters))
    timeOutTimer = millis(); //Reset timeOutTimer if new data is entered
    inputCode += key;
    tft.setTextSize(2);
      int x = startX;
        for (int i = 0; i < inputCode.length(); i++) { // Add a star for every button press 
          tft.setCursor(x, 71); //Define cordinates for where the stars should show
          tft.setTextColor(defaultTextColor);
          tft.print("*");
          x += 20; //increase x-coordinate for next star
        }
  }

  if(key == '#' && (inputCode.length() > 0)){
  if (inputCode.equals(pinCodes[checkValue])) {  //correct code for card
  accessTimer = millis();
    tft.fillScreen(ST77XX_BLACK);
    tft.drawBitmap(0, 0, epd_bitmap_accessGranted, 160, 128, ST77XX_GREEN);
    //Add code for opening gate for a few sec 
    client.publish("accessGranted", Access.c_str());
    inputCode = "";
    caseState = 8;
  }

else {  //incorrect code for card, go to "access denied" (case 9)
    accessTimer = millis();
    tft.fillScreen(ST77XX_BLACK);
    tft.drawBitmap(0, 0, epd_bitmap_accessDenied, 160, 128, ST77XX_RED); 
    //Deny access
    client.publish("accessDenied", Access.c_str());
    
    inputCode = "";
    caseState = 9;

}
  }

    if(key == 'B'){ //go back to homescreen if B is pressed
    caseState = 2; //back to homescreen
    inputString = "";
    inputCode = "";
  }

  if(millis() - timeOutTimer >= timeOutTime){ //Go to timeout screen if no new data is unput over 
    tft.fillScreen(ST77XX_BLACK);
    tft.drawBitmap(0, 0, epd_bitmap_timedOut, 160, 128, ST77XX_WHITE);
    caseState = 10;
    timeOutShowTimer = millis();
  }

  break;

  case 8:
  //Serial.println("case 8");
  if(millis() - accessTimer >= accessTime){
    caseState = 2; //back to homescreen
  }
  break;

  case 9:

  //Serial.println("case 9");
  if(millis() - accessTimer >= accessTime){
    caseState = 2; //back to homescreen
  }
  break;

  case 10: //Timedout
  //Serial.println("case 10");
  if (millis() - timeOutShowTimer >= timeOutShowTime){
    caseState = 2; //back to homescreen
  }
  break;

  case 11: //unknown card
  //Serial.println("case 11");
  rfidAllow = millis();
  if (millis() - cardUnknownTimer >= cardUnknownDisplayTime){
  
  caseState = 2; //back to homescreen
}
  break;
  case 12:
  if(key){
    caseState = 2;
    energySaveTimer = millis();
  }

}
}
