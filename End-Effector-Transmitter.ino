#include <WiFi.h>
#include <esp_now.h>

// Replace with your receiver's MAC address for unicast; 
// for testing, we'll use the broadcast address (all 0xFF).
uint8_t receiverMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Callback to display send status
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP-NOW Sender Starting...");

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while (true); // halt if initialization fails
  }
  esp_now_register_send_cb(onDataSent);

  // Register peer using broadcast (or update receiverMACAddress for unicast)
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverMACAddress, 6);
  peerInfo.channel = 0;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add ESP-NOW peer");
    while (true);
  }
}

void loop() {
  // ----- 1. Send a periodic test message as sensor ID 1 -----
  String periodicMsg = "$1~Hello from Sensor 1`";
  int len1 = periodicMsg.length() + 1;
  char buffer1[len1];
  periodicMsg.toCharArray(buffer1, len1);
  esp_now_send(receiverMACAddress, (uint8_t *)buffer1, len1);
  Serial.print("Sent periodic message: ");
  Serial.println(periodicMsg);

  // ----- 2. Check for Serial input and send it as sensor ID 5 -----
  if (Serial.available() > 0) {
    String serialInput = Serial.readStringUntil('\n');
    serialInput.trim();
    if (serialInput.length() > 0) {
      String serialMsg = "$5~" + serialInput + "`";
      int len2 = serialMsg.length() + 1;
      char buffer2[len2];
      serialMsg.toCharArray(buffer2, len2);
      esp_now_send(receiverMACAddress, (uint8_t *)buffer2, len2);
      Serial.print("Sent serial message: ");
      Serial.println(serialMsg);
    }
  }

  delay(2000);  // Wait 2 seconds before next loop iteration
}
