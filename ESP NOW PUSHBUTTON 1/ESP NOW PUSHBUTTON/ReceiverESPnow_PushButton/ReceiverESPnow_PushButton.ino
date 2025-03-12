
#include <esp_now.h>
#include <WiFi.h>
const int lightME = 2;

struct __attribute__((packed)) dataPacket {
  
  int state;
 
};
// callback function that will be executed when data is received
 void OnDataRecv(const esp_now_recv_info* info, const uint8_t *incomingData, int len) {
  
  dataPacket packet;
  memcpy(&packet, incomingData, sizeof(packet));
  Serial.print("button: ");
  Serial.println(packet.state);
  digitalWrite(lightME,(packet.state));
  
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(lightME, OUTPUT);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}
