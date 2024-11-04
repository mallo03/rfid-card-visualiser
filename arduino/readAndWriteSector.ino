// To be used in conjunction with Arduino UNO and RFID-RC522 Module within the ARDUINO IDE

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//intialise
void setup() {
    Serial.begin(9600);
    while (!Serial);
    SPI.begin();
    mfrc522.PCD_Init();

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Place Card A on the reader to read data."));
}

void loop() {
    static bool cardARead = false;
    static byte bufferA[3][16]; // buffer to store data read from Card A (blocks 4, 5, 6)
    
    //card A reading
    if (!cardARead && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        Serial.println(F("Reading data from Card A (blocks 4, 5, and 6)..."));
        
        for (byte blockAddr = 4; blockAddr <= 6; blockAddr++) {
            // Authenticate and read data
            MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 7, &key, &(mfrc522.uid));
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("Authentication failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
                return;
            }

            byte buffer[18];
            byte size = sizeof(buffer);
            status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("Read failed at block ")); Serial.println(blockAddr);
                Serial.println(mfrc522.GetStatusCodeName(status));
                return;
            }

            memcpy(bufferA[blockAddr - 4], buffer, 16);//store data in buffer
            Serial.print(F("Data from block ")); Serial.print(blockAddr); Serial.print(F(": "));
            dump_byte_array(buffer, 16);
            Serial.println();
        }
        
        cardARead = true;
        Serial.println(F("Now, place Card B on the reader to write data."));
        
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();   
    }

    if (cardARead && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        Serial.println(F("Writing data to Card B (blocks 4, 5, and 6)..."));

        for (byte blockAddr = 4; blockAddr <= 6; blockAddr++) {
            MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 7, &key, &(mfrc522.uid));
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("Authentication failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
                return;
            }

            status = mfrc522.MIFARE_Write(blockAddr, bufferA[blockAddr - 4], 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("Write failed at block ")); Serial.println(blockAddr);
                Serial.println(mfrc522.GetStatusCodeName(status));
                return;
            }

            Serial.print(F("Data written to block ")); Serial.print(blockAddr); Serial.print(F(": "));
            dump_byte_array(bufferA[blockAddr - 4], 16);
            Serial.println();
        }

        Serial.println(F("Data successfully copied from Card A to Card B."));
        
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();

        cardARead = false; 
    }
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
