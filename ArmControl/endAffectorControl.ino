//---libraries---
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);


const int mPin1 = 25;
const int mPin2 = 33;
const int enPin = 26;

const int potPin = 32;
const int buttonCWPin = 14;
const int buttonACWPin = 12;

const int maxPot = 4095;
const int margin=100;
void setup() {
  // sets the pins as outputs:
  pinMode(potPin, INPUT);
  pinMode(mPin1, OUTPUT);
  pinMode(mPin2, OUTPUT);
  pinMode(enPin, OUTPUT);
  pinMode(buttonCWPin, INPUT);
  pinMode(buttonACWPin, INPUT);

  //serial
  Serial.begin(115200);

  //setting up rfid
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Scan your RFID card to read...");
  
  // testing
  Serial.println("Testing DC Motor...");
  digitalWrite(enPin,HIGH);
}

void loop() {
  int potPos = analogRead(potPin);
  //Serial.println(analogRead(potPin));
  if (digitalRead(buttonCWPin)==HIGH && potPos<maxPot-margin){
    
    digitalWrite(mPin1,HIGH);
    digitalWrite(mPin2,LOW);
    //Serial.println("CW button pressed");
  }
  else if (digitalRead(buttonACWPin)==HIGH && potPos>margin){
    digitalWrite(mPin1,LOW);
    digitalWrite(mPin2,HIGH);
    //Serial.println("ACW button pressed");
    }
  else {
    digitalWrite(mPin1,LOW);
    digitalWrite(mPin2,LOW);
    //Serial.println("No buttons pressed");
  }
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String message = "";
  byte buffer[18];  // Buffer for reading data
  byte size = sizeof(buffer);
  MFRC522::StatusCode status;

  byte sectorKey[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  MFRC522::MIFARE_Key key;
  memcpy(key.keyByte, sectorKey, 6);

  int readBlock = 1;  // Start reading from Block 1

  while (readBlock < 64) {
    if (readBlock % 4 == 3) {
      readBlock++;  // Skip sector trailer blocks
      continue;
    }

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, readBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Authentication failed at block ");
      Serial.println(readBlock);
      break;
    }

    status = mfrc522.MIFARE_Read(readBlock, buffer, &size);
    if (status == MFRC522::STATUS_OK) {
      for (byte i = 0; i < 16; i++) {
        if (buffer[i] == 0) continue;  // Skip null bytes
        message += (char)buffer[i];
      }
    } else {
      Serial.print("Read failed at block ");
      Serial.println(readBlock);
      break;
    }

    readBlock++;
  }

  Serial.print("Message Read: ");
  Serial.println(message);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
