
#include <esp_now.h>
#include <WiFi.h>
const int pushDown = 1;
// RECEIVER MAC ADDRESS: 60:55:f9:af:83:ec
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x60, 0x55, 0xf9, 0xaf, 0x83, 0xec};
struct __attribute__((packed)) dataPacket {
  int state ;
};

esp_now_peer_info_t peerInfo;

// callback when data is sent - I CAN CHANGE THIS FUNCTION BELOW
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  pinMode(pushDown,INPUT_PULLUP);
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  //pinMode(pushDown, INPUT);
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {

   dataPacket packet;

  packet.state = digitalRead(pushDown);

  esp_now_send(broadcastAddress, (uint8_t *) &packet, sizeof(packet));

  delay(30);
 
}