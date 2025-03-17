/*Il fucile è la board B*/

#include <esp_now.h>
#include <WiFi.h>
#include "Wire.h"
#include <MPU6050_light.h>

int trigger = 17; //pin dell 
int luce = 34;// led che simula il flash di uno sparo
 MPU6050 mpu(Wire);
 unsigned long timer = 0;
//indirizzo MAC della board Base a cui mandare il segnale
uint8_t broadcastAddress[] = {0x3c, 0x61, 0x05, 0x3e, 0xbd, 0x60};
//stinga da conservare se l' invio del messaggio palle
String success;


//struttura dati che contiene i messaggi da mandare all altra scheda
//__attribute__((packed)) fa semplicemente in modo che occupi il meno spazio possibile
struct __attribute__((packed)) dataPacket {
  bool blank;//idica se il proittile sparato è live o blank
  int aiming;// intero tra 1 e 4 che inidca a che giocatore stava puntando il fucile al momento dello sparo
};


//struttura dati che contiene i dati in arrivo dall altra scheda
struct __attribute__((packed)) datiRicevuti {
  int mag[8];
};


//info sulla sua scheda peer
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
  memcpy(&incomingData, incomingData, sizeof(incomingData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  datiRicevuti.mag = incomingData.mag;
  
}

 void setup() {
   Serial.begin(115200);
   pinMode(trigger, INPUT_PULLUP);
   pinMode(luce, OUTPUT);
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
    // Send message via ESP-NOW
  if (digitalRead(trigger) == HIGH) {
    //aggiungere la sequenza che dice l angolo di puntamento al momento di premuta del grilleto
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dataPacket, sizeof(dataPacket));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  }
   
 }
 /*cose da aggiungere:
 -fare in modo che alla pressione del grilleto venga mandato l angolo di puntamento alla base
 -dioporco
-riscrivere sto codice da capo perchè non ho idea che cazzo sia rimasto da aggiustare*/