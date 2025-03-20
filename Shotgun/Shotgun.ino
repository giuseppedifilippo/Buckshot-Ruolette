/*Il fucile è la board B*/
/*questa scheda si occupa di calcolare l angolo di puntamento del fucile e mandare la direzione alla scheda base che gestisce la turnazione 
e i danni e il calcolo delle shell*/

//COSE DA AGGIUNGERE
//comunicazione per dire è stato cambiato il giocatore e deve ricalibrare l mpu
//dovrei cambiare la struttura dati che il fucile riceve
#include <esp_now.h>
#include <WiFi.h>
#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;
int trigger_pin = 15;  //pin su cui è collegato l'interruttore del grilletto
int light = 34;        //pin per i led che simulano il flash del fucile

//indirizzo MAC della board Base a cui mandare il segnale
uint8_t broadcastAddress[] = { 0x3c, 0x61, 0x05, 0x3e, 0xbd, 0x60 };

//stinga da conservare se l' invio del messaggio palle
String success;

//il messaggio deve essere in una struct dato che deve essere passato per riferimento
struct __attribute__((packed)) dataPacket {
  int shot;  //shot indica se è stato sparato un live(true) o un blank(false)
  //valore tra 1 e 4, indica a quale giocatore il fucile punta al momento del fuoco
  //1 vuol dire che il fucile è puntato alla propria sinistra
  //2 vuol dire che è puntato a se stesso
  //3 vuol dire che è puntato al giocatore alla propria destra
  //4 vuol dire che è puntato al giocatore di fronte a se stesso
  int aiming_at;
};

dataPacket packet;  //devi ridichiarare la struct dopo aver fatto la typedef
struct __attribute__((packed)) incomingData {
  bool shell;  //inidca se la prossima shell è live
  bool next;   //indica se è cambiato il giocatore ed è necessario aggiornare la posizione del fucile
};
incomingData in_arrivo;

//info sulla sua scheda peer
esp_now_peer_info_t peerInfo;

// Callback quando i dati sono mandati
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received 
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingData, incomingData, sizeof(incomingData));
  Serial.print("Bytes received: ");
  Serial.println(len);
}

void setup() {
  Serial.begin(115200);
  pinMode(trigger_pin, INPUT_PULLDOWN);
  pinMode(light, OUTPUT);
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {}  // stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets();  // ricalibra il sensore
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
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}



void loop() {
  mpu.update();
  if (in_arrivo.next == true) {
    mpu.calcOffsets();  //quando cambia giocatore ricalibra il giroscopio
  }
  if ((millis() - timer) > 10) {  //aggiorna la posizione ogni 10ms
    if (mpu.getAngleZ() < 20 && mpu.getAngleZ() > -20 && mpu.getAngleX() < 90) {
      packet.aiming_at = 4;
      Serial.println("aiming at front");
    } else if (mpu.getAngleZ() < -20 && mpu.getAngleZ() > -60 && mpu.getAngleX() < 90) {
      packet.aiming_at = 3;
      Serial.println("aiming at right");
    } else if (mpu.getAngleZ() < 60 && mpu.getAngleZ() > 20  && mpu.getAngleX() < 90) {
      packet.aiming_at = 1,
      Serial.println("aiming at left");
    } if (mpu.getAngleX() > 90 && mpu.getAngleX() < 180) {
      packet.aiming_at = 2;
      Serial.println("suicide mode");
    }
  timer = millis();
  }
  //sequenza di azioni che avvengono alla pressione del grilletto
  if (digitalRead(trigger_pin) == HIGH) {
    // Send message via ESP-NOW
    digitalWrite(light, HIGH);
    delay(100);
    digitalWrite(light, LOW);
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));  //fixare richiede un espressine prima della virgola


    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
  }
}
