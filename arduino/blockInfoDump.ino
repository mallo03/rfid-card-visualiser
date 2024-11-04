// To be used in conjunction with Arduino UNO and RFID-RC522 Module within the ARDUINO IDE

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          
#define SS_PIN          10         

MFRC522 mfrc522(SS_PIN, RST_PIN);  

void setup() {
    Serial.begin(9600);        
    while (!Serial);        
    SPI.begin();            
    mfrc522.PCD_Init();    
    delay(4);                
    mfrc522.PCD_DumpVersionToSerial();    
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    Serial.print(F("Card UID: "));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    Serial.print(F("Card SAK: "));
    Serial.println(mfrc522.uid.sak, HEX);

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.print(F("PICC type: "));
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // loop through blocks 0 to 63
    for (byte block = 0; block < 64; block++) { 
        MFRC522::StatusCode status;
        byte buffer[18]; 
        byte size = sizeof(buffer);
        MFRC522::MIFARE_Key key;
        for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; 

        byte trailerBlock = block % 4 == 3; // trailer blocks 3, 7, 11, and 15
        status = mfrc522.PCD_Authenticate(trailerBlock ? MFRC522::PICC_CMD_MF_AUTH_KEY_B : MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("Authentication failed for block ")); 
            Serial.println(block);
            continue; //skip to next block
        }

        // read
        status = mfrc522.MIFARE_Read(block, buffer, &size);
        if (status == MFRC522::STATUS_OK) {
            Serial.print(F("Block ")); Serial.print(block); Serial.print(F(": "));
            for (byte i = 0; i < 16; i++) {
                Serial.print(buffer[i] < 0x10 ? " 0" : " ");
                Serial.print(buffer[i], HEX);
            }
            Serial.println();
        } else {
            Serial.print(F("Failed to read block ")); Serial.println(block);
        }
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}
