#include <FastLED.h>
#include "servo_move.h"

#define n_led 12
#define n_player 4
#define data_pin_g1 3
#define data_pin_g2 4
#define data_pin_g3 5
#define data_pin_g4 6
#define data_pin_rele1 7
#define data_pin_rele2 8
#define data_pin_rele3 9
#define data_pin_rele4 10
#define time_zapping 2000

int life[n_player]; //inizializzo un array di vite che contiene il numero di queste per ogni giocatore
CRGB leds[n_player][n_led];
String action="";
String ins[3]; //array che contiene le parti della stringa inserita in seriale
//lifes and player indicated by instruction
int lifes_ins = 0;
int player = 0;
//numero del giocatore all'interno dell'array di giocatori
int player_matrix = 0;

//controlla che venga inviato un messaggio in seriale e suddivide la stringa nelle tre parti dell'istruzione
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

//aggiorna il numero delle vite quando viene inviato un messaggio in seriale
//quando la vita viene persa richiama la funzione zapping per folgorare il giocatore
//il messaggio è formattato così: istruzione numero_vite numero_giocatore. Le istruzioni possono essere "rm" per rimuovere e "add" per aggiungere
void update_life(){
  if(ins[0]!="" && ins[1]!="" && ins[2]!=""){
    String cmd = ins[0];
    lifes_ins = ins[1].toInt(); 
    player = ins[2].toInt();
    player_matrix = player -1;

    if (cmd == "rm") {
      life[player_matrix] -= lifes_ins;
      if (life[player_matrix] < 0){
        life[player_matrix] = 0;
      } 
      zapping(player);
    } else if (cmd == "add" && (life[player_matrix] + lifes_ins) <= n_led) {
      life[player_matrix] += lifes_ins;
    }
    action = "";
  } else{
    return;
  }
}

//accende un numero di led corrispondente al numero di vite (N.B. ogni vita è indicata da due led)
void update_led(){
  for(int i = 0; i < n_led; i++) {
    if(i < life[player_matrix]*2){
      leds[player_matrix][i] = CRGB::Green;
    } else {
      leds[player_matrix][i] = CRGB::Black;
    }
  }
  FastLED.show();
}

//resetta il numero di led accesi spegnendoli tutti
void clearLed(){
  for(int j=0; j<n_player; j++){
    for(int i=0; i<n_led; i++){
      leds[j][i]=CRGB::Black;
    }
  }
  FastLED.show();
}


//resettta i led del giocatore passato come parametro al numero di vite passate
//se il valore passato è 0 resetta le vite di tutti i giocatori
void reset(int n_vite, int player){
  clearLed();
  if(n_vite <=6){
    n_vite*=2;
    if(player==0){
      for(int i=0; i<n_player; i++){
        for(int j=0; j<n_vite; j++){
          leds[i][j]=CRGB::Green;
        }
      }
    }else{
      for(int j=0; j<n_vite; j++){
          leds[player][j]=CRGB::Green;
      }
    }
    FastLED.show();
  } else{
    return;
  }
}

//invia un segnale ai relè per permettere il passaggio della corrente attraverso gli elettrodi
void zapping(int player){
  player += 6;
  digitalWrite(player, HIGH);
  delay(time_zapping);
  digitalWrite(player, LOW);
}

void setup() {
  setup_servos();
  pinMode(data_pin_rele1, OUTPUT);
  pinMode(data_pin_rele2, OUTPUT);
  pinMode(data_pin_rele3, OUTPUT);
  pinMode(data_pin_rele4, OUTPUT);
  FastLED.addLeds<WS2812B, data_pin_g1, RGB>(leds[0], n_led);
  FastLED.addLeds<WS2812B, data_pin_g2, RGB>(leds[1], n_led);
  FastLED.addLeds<WS2812B, data_pin_g3, RGB>(leds[2], n_led);
  FastLED.addLeds<WS2812B, data_pin_g4, RGB>(leds[3], n_led);  
  clearLed();
  for(int i=0; i<n_player; i++){
    life[i]=n_led;
  }
  reset(6);
  Serial.begin(9600);
}

void loop() {
  checkSerial();
  if(action != ""){
    if(action[0]=='s' && action[1]=='h'){
      mostra_shell(action.substring(3));
    }
    update_life();
    update_led();
  }
}