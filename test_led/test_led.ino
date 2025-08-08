#include <FastLED.h>

#define data_pin 3
#define n_led 12

CRGB led[12];

void setup(){
  FastLED.addLeds<WS2812B, data_pin, RGB>(led, n_led);
}

void loop(){
  for(int i=0; i<12; i+=2){
    fill_solid(&(led[i]), 2, CRGB::Green);
    FastLED.show();
    delay(1000);
  }
  for(int i=0; i<12; i++){ 
    fill_solid(led, n_led, CRGB::Black);
  }
  FastLED.show();
}