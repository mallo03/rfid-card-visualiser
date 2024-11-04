// To be used in conjunction with Arduino UNO and RFID-RC522 Module within the ARDUINO IDE

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Place the card on the reader to write random data...");
  
  randomSeed(analogRead(0)); // seed rand gen
}

void loop() {

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  for (byte block = 4; block < 7; block++) { 
    byte randomData[16];
    generateRandomData(randomData); 

    if (writeBlock(block, randomData)) {
      Serial.print("Random data written successfully to Block ");
      Serial.println(block);
    } else {
      Serial.print("Failed to write to Block ");
      Serial.println(block);
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(2000); 
}
// write to block
bool writeBlock(byte block, byte *buffer) {
  MFRC522::StatusCode status;
  byte trailerBlock = (block / 4) * 4 + 3;

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for block ");
    Serial.println(block);
    return false;
  }


  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Failed to write block ");
    Serial.println(block);
    return false;
  }
  return true;
}

void generateRandomData(byte *buffer) {
  for (int i = 0; i < 16; i++) {
    buffer[i] = random(0, 256); 
  }
}
