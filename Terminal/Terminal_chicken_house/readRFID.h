
void readRFID() {
if(millis() - rfidAllow >= 1000){ // denying RFID reading for 1 sec after it reads a card..

    
  
    if (rfid.PICC_ReadCardSerial()) {
        
    rfidAllow = millis();
        bool knownCard = false; // Set to false at the beginning of the function
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        Serial.print("RFID/NFC Tag Type: ");
        Serial.println(rfid.PICC_GetTypeName(piccType));

        // print UID in Serial Monitor in hex format, this UID can be added underk nown cards in variables
        Serial.print("UID: ");
        for (int i = 0; i < rfid.uid.size; i++) {
            cardUID[i] = rfid.uid.uidByte[i];
            Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();

        rfid.PICC_HaltA(); // Halt PICC
        rfid.PCD_StopCrypto1(); // Stop encryption on PCD

        knownCard = false; // Set to false at the beginning of the function

        for (int j = 0; j < sizeof(knownCards) / sizeof(knownCards[0]); j++) {
            

            for (int i = 0; i < sizeof(cardUID); i++) {
                if (cardUID[i] == knownCards[j][i]) {
                    knownCard = true; // If a byte doesn't match, it's not a known card
                    break; // Exit the loop
                }
            }

            if (knownCard) {
                checkValue = j;
                timeOutTimer = millis(); // Setting timer
                tft.fillScreen(ST77XX_BLACK);
                Serial.print("kjent kort");
                tft.drawBitmap(0, 0, epd_bitmap_entercode, 160, 128, ST77XX_WHITE);
                caseState = 6;
                break;
            }
        }

        if (!knownCard) {
            tft.fillScreen(ST77XX_BLACK);
            tft.drawBitmap(0, 0, epd_bitmap_cardUnknown, 160, 128, ST77XX_WHITE);
            Serial.print("ikke kjent kort");
            cardUnknownTimer = millis(); // Setting timer
            caseState = 11;
            Serial.println("dette kommer fra readRFID: ");
            Serial.print(caseState);
        }
    }
}
}




    