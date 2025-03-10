/*Il fucile è la board B*/

#include <esp_now.h>
#include <WiFi.h>
#include "Wire.h"
#include <MPU6050_light.h>


 MPU6050 mpu(Wire);
 unsigned long timer = 0;
//indirizzo MAC della board Base a cui mandare il segnale
uint8_t broadcastAddress[] = {0x3c, 0x61, 0x05, 0x3e, 0xbd, 0x60};
//stinga da conservare se l' invio del messaggio palle
String success;


typedef int Mag[8];
Mag incoming;

//info sulla sua schedapeer
esp_now_peer_info_t peerInfo;

// Callback quando i dati sono mandati
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming, incomingData, sizeof(incoming));
  Serial.print("Bytes received: ");
  Serial.println(len);
  
  
}

 void setup() {
   Serial.begin(115200);
   Wire.begin();
 byte status = mpu.begin();
   Serial.print(F("MPU6050 status: "));
   Serial.println(status);
   while (status != 0) { } // stop everything if could not connect to MPU6050
 Serial.println(F("Calculating offsets, do not move MPU6050"));
   delay(1000);
   mpu.calcOffsets(); // ricalibra il sensore
   Serial.println("Done!n");


// Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
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
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

 }
 void loop() {
   mpu.update();
 if ((millis() - timer) > 10) { //aggiorna la posizione ogni 10ms
     if (mpu.getAngleZ() < 20 && mpu.getAngleZ() > -20) {
      Serial.println("aiming at front");
     } else if (mpu.getAngleZ() < -20 && mpu.getAngleZ() > -60) {
      Serial.println("aiming at right");
     } else if (mpu.getAngleZ() < 60 && mpu.getAngleZ() > 20) {
      Serial.println("aiming at left");
     }
     timer = millis();
   }
   
 }