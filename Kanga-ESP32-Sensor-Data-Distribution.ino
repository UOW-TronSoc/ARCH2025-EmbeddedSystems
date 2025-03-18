#include <WiFi.h>
#include <esp_now.h>

// ESP-NOW receive callback using the current signature
void onDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  // Optionally, print the sender's MAC address:
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
           recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
  //Serial.print("Received packet from: ");
  //Serial.println(macStr);

  // Print the incoming message
  //Serial.print("Message: ");
  for (int i = 0; i < len; i++) {
    Serial.print((char)incomingData[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver starting...");

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    while(1);
  }

  // Register the receive callback
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  // Nothing needed here—messages are processed in the callback.
  delay(100);
}
