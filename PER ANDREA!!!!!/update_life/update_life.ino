#include <FastLED.h>

#define n_led 8
#define data_pin 3

int life=8;
CRGB leds[n_led];
String action="";
String ins[3];
//lifes and player indicated by instruction
int lifes_ins = 0;
int player = 0;

void checkSerial(){
  if (Serial.available()) {
    action = Serial.readStringUntil('\n');
    action.trim();

    // divide the instruction in 3 parts
    for (int i = 0; i < 3; i++) {
      int index = action.indexOf(' ');
      ins[i] = action.substring(0, index);
      action = action.substring(index + 1);
    }
  }
}

void update_life(){
  lifes_ins = ins[1].toInt();
  player = ins[2].toInt(); // utile se hai un array di vite

  String cmd = ins[0];

  if (cmd == "rm") {
    life -= lifes_ins;
  } 
  else if (cmd == "add" && (life + lifes_ins) <= n_led) {
    life += lifes_ins;
  }
  action = "";
}

void update_led(){
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

void setup() {
  FastLED.addLeds<WS2812B, data_pin, RGB>(leds, n_led);  
  Serial.begin(9600);
  clearLed();
}

void loop() {
  checkSerial();
  update_led();
  if(action != ""){
    update_life();
  }
}