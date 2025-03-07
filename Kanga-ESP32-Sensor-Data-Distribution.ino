#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <string>

using namespace std;
string currentRfidMessage;
string currentPotMessage;
string currentImuMessage;
string currentNirMessage;

// Updated callback function with correct signature and message parsing
void onDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  // Extract sender MAC address from recv_info
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
           recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
  Serial.print("Received packet from: ");
  Serial.println(macStr);

  // Build the received message string as an Arduino String
  String receivedMsg = "";
  for (int i = 0; i < len; i++) {
    receivedMsg += (char)incomingData[i];
  }
  Serial.print("Received wireless data: ");
  Serial.println(receivedMsg);

  // Parse the message: expected format is "$<sensorID>~<data>`"
  if (receivedMsg.charAt(0) == '$') {
    int tildeIndex = receivedMsg.indexOf('~');
    if (tildeIndex == -1) {
      Serial.println("Error: Tilde separator not found.");
      return;
    }
    String sensorIdStr = receivedMsg.substring(1, tildeIndex);
    int sensorId = sensorIdStr.toInt();
    int backtickIndex = receivedMsg.lastIndexOf('`');
    if (backtickIndex == -1) {
      Serial.println("Error: Ending backtick not found.");
      return;
    }
    String dataStr = receivedMsg.substring(tildeIndex + 1, backtickIndex);

    // Store the data in the appropriate global variable based on sensorID
    switch (sensorId) {
      case 1:
        currentRfidMessage = string(dataStr.c_str());
        break;
      case 2:
        currentPotMessage = string(dataStr.c_str());
        break;
      case 3:
        currentImuMessage = string(dataStr.c_str());
        break;
      case 4:
        currentNirMessage = string(dataStr.c_str());
        break;
      default:
        Serial.println("Error: Unknown sensor ID received.");
    }
  } else {
    Serial.println("Error: Message does not start with '$'.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver Starting...");

  // Set WiFi to station mode and initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while (true);  // halt if ESP-NOW fails to initialize
  }
  
  // Register the receive callback with the updated signature
  esp_now_register_recv_cb(onDataRecv);
  
  // Initialize I²C for sensors (adjust SDA/SCL pins as needed)
  Wire.begin();
  
  // Initialize sensors (GY-85, AS7265x) as needed
  // e.g., initGY85(); or as required by your libraries
}

void sendMessage(int sensorID, string message) {
  // Build the message string with the specified format: $<sensorID>~<data>`
  String msg = String("$") + sensorID + "~" + message.c_str() + "`";
  Serial.println(msg);
}

void loop() {
  // IMU GY85 function
  // ENTER FUNCTION
  // currentImuMessage = "";
  
  // NIR function
  // ENTER FUNCTION
  // currentNirMessage = "";
  
  // For demonstration, send out the stored messages.
  // Messages received via ESP-NOW will update currentRfidMessage and currentPotMessage.
  sendMessage(1, currentRfidMessage);
  sendMessage(2, currentPotMessage);
  sendMessage(3, currentImuMessage);
  sendMessage(4, currentNirMessage);
  delay(50);  // Adjust delay as required
}
