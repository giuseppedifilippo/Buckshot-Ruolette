#include <FastLED.h>

#define n_led 8
#define data_pin 3

int life=8;
CRGB leds[n_led];
String action="";

void setup() {
  FastLED.addLeds<WS2812B, data_pin, RGB>(leds, n_led);  
  Serial.begin(9600);
  clearLed();
}

void loop() {
  checkSerial();
  update_life();
}

void checkSerial(){
  if(Serial.available()){
    action = Serial.readString();
    action.trim(); //to remove all the spaces
    if(life>0){
      if (action == "rm"){ //rm command to remove life
        life -= 1;
      }else if(action == "add" && life < n_led){ //add command to add life
        life ++;
      }
    }
  }
}

void update_life(){
  for(int i = 0; i<n_led; i++){
    if(i<life){
      leds[i] = CRGB::Green;
    }else{
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
}

void clearLed(){
  for(int i=0; i<n_led; i++){
    leds[i]=CRGB::Black;
  }
}