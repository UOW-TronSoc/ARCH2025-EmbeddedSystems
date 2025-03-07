#include <WiFi.h>
#include <esp_now.h>
#include <SPI.h>
#include <MFRC522.h>

// Define pins for the RFID RC522
#define RST_PIN  5    // Reset pin (adjust as needed)
#define SS_PIN   4    // SPI Slave Select pin

// Define analog pin for the linear potentiometer
#define POT_PIN  34   // Adjust based on your wiring

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Replace with the receiver ESP32's MAC address (6 bytes)
uint8_t receiverMACAddress[] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC}; // <-- Update this!

// Callback function when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Transmitter Starting...");

  // Initialize SPI and RFID reader
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RC522 RFID reader initialized.");

  // Set WiFi to station mode and initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while (true); // halt
  }
  
  esp_now_register_send_cb(onDataSent);
  
  // Register the peer (receiver)
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMACAddress, 6);
  peerInfo.channel = 0;  // use current channel
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add ESP-NOW peer");
    while (true); // halt
  }
}

void loop() {
  // --- Read from RFID RC522 ---
  // Check if a new RFID card is present
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String rfidData = "";
    // Concatenate UID bytes into a string (in HEX format)
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) {
        rfidData += "0";
      }
      rfidData += String(mfrc522.uid.uidByte[i], HEX);
    }
    rfidData.toUpperCase();
    // Construct message using sensor id 1 (for RFID) ending with backtick `
    String message = "$1~" + rfidData + "`";
    Serial.print("Sending RFID data: ");
    Serial.println(message);
    
    // Convert String to char array and send via ESP-NOW
    int msgLen = message.length() + 1;
    char msgBuffer[msgLen];
    message.toCharArray(msgBuffer, msgLen);
    esp_now_send(receiverMACAddress, (uint8_t *)msgBuffer, msgLen);
    
    // Halt the card until it is removed
    mfrc522.PICC_HaltA();
    delay(500);
  }
  
  // --- Read from Linear Potentiometer ---
  int potValue = analogRead(POT_PIN);
  // Construct message using sensor id 2 (for potentiometer) ending with backtick `
  String potMessage = "$2~" + String(potValue) + "`";
  Serial.print("Sending Potentiometer data: ");
  Serial.println(potMessage);
  
  int potMsgLen = potMessage.length() + 1;
  char potMsgBuffer[potMsgLen];
  potMessage.toCharArray(potMsgBuffer, potMsgLen);
  esp_now_send(receiverMACAddress, (uint8_t *)potMsgBuffer, potMsgLen);
  
  delay(1000);  // Adjust loop delay as needed
}
