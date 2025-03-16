/*codice per la scheda base che si occupa della gestione 
dei turni danni e altro, genera una sequenza di shell e le manda alla 
scheda sul fucile che a sua volta manderà un messaggio quando un colpo sarà 
sparato carico o a salve*/

#include <FastLED.h>
#define NUM_LEDS 8
#define LED_PIN 4 //gpio4 su esp32
CRGB leds[NUM_LEDS];

//variabili per gestire le cartucce a la loro generazione random
int mag[8]; 
int numero;
bool enter = true;
int j = 0;

//struct che contiene la seuenza delle munizioni da mandare all esp sul fucile
struct __attribute__((packed)) dataPacket {
  int mag[8];
}

 void OnDataRecv(const esp_now_recv_info* info, const uint8_t *incomingData, int len) {
  
  dataPacket packet;
  memcpy(&packet, incomingData, sizeof(packet));
  leds[j] = CRGB ( 0, 0, 0 );
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
}

void loop() {

/*genero un numero random da 0 a 255 che viene convertito in binario 
la sequenza di 0 e 1 indica la sequenza dei proiettili(0 a salve, 1 carico, 2 per colpo già sparato)  */
if (digitalRead(10) == HIGH || enter == true) {
  numero  = random(0, 255);
  enter = false;
  for (int h = 0; h < 8 ; h++) {
    mag[h] = bitRead(numero, h);
  }
  j = 0;
}


for (int i = 0; i < 8 ; i++) {
   if (mag[i] == 0) {
    leds[i] = CRGB ( 0, 0, 255);
  } else if  (mag[i] == 1 ) {
    leds[i] = CRGB ( 255, 0, 0 );
  }
}

FastLED.show();
if (digitalRead(11) == HIGH ) { //shot fired
  leds[j] = CRGB ( 0, 0, 0 );
  FastLED.show();
  mag[j] = 2;
  j++;
} 

//per debugging; stampa la sequenza del caricatore
/*for (int j = 0 ; j < 8; j++){
Serial.print(mag[j]);
}
Serial.println();*/

delay(3000);
}


