#include <FastLED.h>

#define n_led 12
#define n_player 4
#define data_pin_g1 3
#define data_pin_g2 4
#define data_pin_g3 5
#define data_pin_g4 6

int life[n_player]; //inizializzo un array di vite che contiene il numero di queste per ogni giocatore
CRGB leds[n_player][n_led];
String action="";
String ins[3]; //array che contiene le parti della stringa inserita in seriale
//lifes and player indicated by instruction
int lifes_ins = 0;
int player = 0;
//numero del giocatore all'interno dell'array di giocatori
int player_matrix = 0;

void checkSerial(){
  //in questo modo l'esecuzione non viene bloccata dall'attesa della seriale
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
  if(ins[0]!="" && ins[1]!="" && ins[2]!=""){
    lifes_ins = ins[1].toInt(); 
    player = ins[2].toInt();
    String cmd = ins[0];
    player_matrix = player -1;

    if (cmd == "rm") {
      life[player_matrix] -= lifes_ins;
      if (life[player_matrix] < 0){
        life[player_matrix] = 0;
      } 
    } else if (cmd == "add" && (life[player_matrix] + lifes_ins) <= n_led) {
      life[player_matrix] += lifes_ins;
    } else if (cmd == "clear") {
      clearLed();
    }
    action = "";
  } else {
    return;
  }
}

void update_led(){
  for(int i = 0; i < n_led; i++) {
    if(i < life[player_matrix]){
      leds[player_matrix][i] = CRGB::Red;
    } else {
      leds[player_matrix][i] = CRGB::Black;
    }
  }
  FastLED.show();
}

void clearLed(){
  for(int j=0; j<n_player; j++){
    for(int i=0; i<n_led; i++){
      leds[j][i]=CRGB::Black;
    }
  }
  FastLED.show();
}

void setup() {
  FastLED.addLeds<WS2812B, data_pin_g1, RGB>(leds[0], n_led);
  FastLED.addLeds<WS2812B, data_pin_g2, RGB>(leds[1], n_led);
  FastLED.addLeds<WS2812B, data_pin_g3, RGB>(leds[2], n_led);
  FastLED.addLeds<WS2812B, data_pin_g4, RGB>(leds[3], n_led);  
  for(int i=0; i<n_player; i++){
    life[i]=n_led;
  }
  Serial.begin(9600);
  clearLed();
}

void loop() {
  checkSerial();
  if(action != ""){
    update_life();
    update_led();
  }
}