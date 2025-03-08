#include <FastLED.h>
#define NUM_LEDS 8
#define LED_PIN 7
CRGB leds[NUM_LEDS];
void setup() {
  // put your setup code here, to run once:
randomSeed(micros());
FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
Serial.begin(9600);
pinMode(10, INPUT);
pinMode(11, INPUT);
}
int mag[8]; 
int numero;
bool enter = true;
int j = 0;


void loop() {
if (digitalRead(10) == HIGH || enter == true) {
  numero  = random(0, 255);
  enter = false;
  for (int h = 0; h < 8 ; h++) {
    mag[h] = bitRead(numero, h);
  }
  j = 0;
}

  // put your main code here, to run repeatedly:
//numero  = random(0, 255);
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
/*for (int j = 0 ; j < 8; j++){
Serial.print(mag[j]);
}
Serial.println();*/

delay(3000);
}
