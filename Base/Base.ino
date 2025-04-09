/*codice per la scheda base che si occupa della gestione 
dei turni danni e altro, genera una sequenza di shell e le manda alla 
scheda sul fucile che a sua volta manderà un messaggio quando un colpo sarà 
sparato carico o a salve*/
/*PER ANDREA*/
/*Se vuoi capire un pochino di più di sta roba cerca FASTLED che è la libreria più usata per controllare led RGB*/
/*e anche ESP NOW che è il protocollo di comunicazione wireless, ci sta una cartella nella repo chiamata PUSHBUTTON che è un esempio di comunicazione 
unidirezionale ma in questo progetto la comunicazione sarà bidirezionale
//BISMILLAH
/*PER ME*/
//RICORDA BENE SE IMPOSTI INPUT_PULLUP VA LOW QUANDO CONNESSO A GND PREMENDO IL PULSANTE,
// SE INVECE IMPOSTI INPUT_PULLDOWN VA HIGH QUANDO PREMI IL PULSANTE CONNESSO A VCC VA HIGH
/*ALTRA COSA DA RICORDARE*/
//SU ESP32 I GPIO CON PULLDOWN INTERNO SONO TUTTI QUELLI NON SPECIALI
#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>
#define NUM_LEDS 8
#define LED_PIN 4  //gpio4 su esp32
CRGB leds[NUM_LEDS];

//variabili per gestire le cartucce a la loro generazione random
int mag[8];
int numero;
bool enter = true;
int j = 0;

//indirizzo mac della scheada del fucile
uint8_t broadcastAddress[] = { 0x3c, 0x61, 0x05, 0x31, 0x67, 0x60 };

esp_now_peer_info_t peerInfo;

//struct che contiene la seuenza delle munizioni da mandare all esp sul fucile
struct __attribute__((packed)) dataPacket {
  bool shell;
  bool next;
};
dataPacket packet;

//struct che contiene le info che riceve dal fucile
struct __attribute__((packed)) incomingData {
  bool shot;
  int aiming_at;
};
incomingData in_arrivo;

//struttura che contiene le info riguardo tutte le informazioni sui giocatori e su come girano i turni
struct sequenza {
  sequenza* next;  //puntatore al prossimo giocatore in senso orario
  sequenza* prev;  //puntatore al prossimo giocatore in senso antioraio
  int current;     //identificativo del giocatore attuale(int da 1 a 4)
  int lives;       //vite che possiede il giocatore attuale
  bool active;     //se il giocatore è attivo
  /*se mai deciderò di voler implementare anche gli item questa struttura sarà da rifare e cambiare completamente la gestione dei giocatori
  anche se o questo o un array ma credo sia peggio
  FUCK IT mi inventerò un modo*/
};
sequenza giocatori;
//funzione che servirà a popolare la sequenza giocatori
sequenza popola_sequenza(int n_giocatori) {

};

// callback when data is sent - I CAN CHANGE THIS FUNCTION BELOW
void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//callback quando vengono ricevuti dati
void OnDataRecv(const esp_now_recv_info* info, const uint8_t* incomingData, int len) {

  dataPacket packet;
  memcpy(&packet, incomingData, sizeof(packet));
  leds[j] = CRGB(0, 0, 0);
  FastLED.show();
  mag[j] = 2;
  j++;
}

void setup() {
  randomSeed(micros());
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  
}

int curr = giocatori.current;  //conservo l id del giocatore attuale
void loop() {

  /*genero un numero random da 0 a 255 che viene convertito in binario 
la sequenza di 0 e 1 indica la sequenza dei proiettili(0 a salve, 1 carico, 2 per colpo già sparato)  */
  if (digitalRead(10) == HIGH || enter == true) {
    numero = random(0, 255);
    enter = false;
    for (int h = 0; h < 8; h++) {
      mag[h] = bitRead(numero, h);
    }
    j = 0;
  }


  for (int i = 0; i < 8; i++) {
    if (mag[i] == 0) {
      leds[i] = CRGB(0, 0, 255);
    } else if (mag[i] == 1) {
      leds[i] = CRGB(255, 0, 0);
    }
  }

  FastLED.show();


  //per debugging; stampa la sequenza del caricatore
  /*for (int j = 0 ; j < 8; j++){ 
Serial.print(mag[j]);
}
Serial.println();*/

  delay(3000);
}

