#include <SPI.h>
#include <MFRC522.h>

#define MaxGrasp 100
#define MinGrasp 0
#define MaxValue 4096
#define MinValue 0

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  // put your setup code here, to run once:
  const int potPin = 34// placeholder pin number for now // this is for the wrist pot
  int potValue = 0;
  int grasp = 0;


  Serial.begin(115200);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522


}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been read
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      //Serial.print("RFID/NFC Tag Type: ");
      //Serial.println(rfid.PICC_GetTypeName(piccType));

      // print NUID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }

  potValue = analogRead(potPin);
  grasp = (MaxGrasp - MinGrasp) ((potValue-MinValue)/(MaxValue-MinValue)) + MinGrasp;
  Serial.print("Grasp size: ");
  Serial.print(grasp);
  Serial.println("%");

  delay(500);
}
