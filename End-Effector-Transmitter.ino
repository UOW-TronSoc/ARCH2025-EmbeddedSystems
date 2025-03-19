#include <WiFi.h>
#include <esp_now.h>
#include <SPI.h>
#include <MFRC522.h>

// RFID configuration
#define SS_PIN 5
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Potentiometer pin and range
const int potPin = 32;
const int maxPot = 4095;

// Broadcast address for ESP‑Now (all 0xFF)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Timer for potentiometer transmission
unsigned long lastPotSend = 0;
const unsigned long potInterval = 250; // Send every 1 second

// ESP‑Now send callback
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP‑Now RFID & Potentiometer Transmitter Starting...");

  // Initialize RFID reader
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID Reader Initialized. Scan a card for instructions...");

  // Set potentiometer pin mode
  pinMode(potPin, INPUT);

  // Set WiFi to station mode and initialize ESP‑Now
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP‑Now");
    while (true);
  }
  esp_now_register_send_cb(onDataSent);

  // Register broadcast peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add ESP‑Now peer");
    while (true);
  }
}

void loop() {
  // --- RFID Reading and Transmission ---
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String rfidData = "";
    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::StatusCode status;
    byte sectorKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    MFRC522::MIFARE_Key key;
    memcpy(key.keyByte, sectorKey, 6);
    
    int readBlock = 1;  // Start reading from Block 1
    while (readBlock < 64) {
      if (readBlock % 4 == 3) {  // Skip trailer blocks
        readBlock++;
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
          rfidData += (char)buffer[i];
        }
      } else {
        Serial.print("Read failed at block ");
        Serial.println(readBlock);
        break;
      }
      readBlock++;
    }
    
    Serial.print("RFID Instruction Read: ");
    Serial.println(rfidData);

    // Format the message as "$1~<instructions>`"
    String message = "$1~" + rfidData + "`";
    Serial.print("Transmitting RFID message: ");
    Serial.println(message);
    
    esp_now_send(broadcastAddress, (uint8_t *)message.c_str(), message.length());
    
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(100);  // Short delay to prevent duplicate reads
  }
  
  // --- Potentiometer Reading and Transmission ---
  if (millis() - lastPotSend >= potInterval) {
    lastPotSend = millis();
    int potValue = analogRead(potPin);
    int percent = potValue;
    String potMessage = "$2~" + String(percent) + "`";
    Serial.print("Transmitting Potentiometer message: ");
    Serial.println(potMessage);
    esp_now_send(broadcastAddress, (uint8_t *)potMessage.c_str(), potMessage.length());
  }
}
