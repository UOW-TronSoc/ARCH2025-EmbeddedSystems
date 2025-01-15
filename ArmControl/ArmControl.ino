#include <SPI.h>
#include <MFRC522.h>

#define MaxGrasp 100 // this needs to be changed once the max grasp is measured
#define MinGrasp 0
#define MaxValue 4096
#define MinValue 0

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  const int potPin = 34// placeholder pin number for now // this is for the wrist pot
  int potValue = 0;
  int grasp = 0; // we could change this to a float if we really cared about that level of precision


  Serial.begin(115200);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522


}

void loop() {
  // RFID stuff
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
      // this printing will have to be replaced

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }


// Linear Sliding potentiometer stuff
  potValue = analogRead(potPin); // read the value of the pot slider
  grasp = (MaxGrasp - MinGrasp) ((potValue-MinValue)/(MaxValue-MinValue)) + MinGrasp; // this maps the input to the range of the 
  //hand so we can just output it as a distance
  Serial.print("Grasp size: ");
  Serial.print(grasp);
  //Serial.println("mm");

  delay(500);
}
