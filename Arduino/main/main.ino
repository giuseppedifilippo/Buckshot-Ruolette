/*
LISTA COMANDI SERIALI:

shl stringa binaria -> comando per mostrare le shell. La stringa binaria indica con uno 0 la shell vuota, con un 1 la shell piena

add n°vite n°giocatore -> comando per aggiungere al giocatore indicato il numero di vite indicato

rm n°vite n°giocatore -> comando per togliere al giocatore indicato il numero di vite indicato

zap n°giocatore -> comando per zappare il giocatore indicato
*/

#include "update_life.h"
#include "servo_move.h"
#include <FastLED.h>

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

CRGB leds[n_player][n_led];

String action="";

//array che contiene le parti della stringa inserita in seriale
String ins_command[3]; 

//inizializzo un array di vite che contiene il numero di queste per ogni giocatore
int life[n_player];

//controlla che venga inviato un messaggio in seriale e suddivide la stringa nelle tre parti dell'istruzione
void checkSerial(){
  //in questo modo l'esecuzione non viene bloccata dall'attesa della seriale
  if (Serial.available()) {
    action = Serial.readStringUntil('\n');
    action.trim();

    //divide la stringa del comando in 3 parti se si tratta di add o rmv 
    //altrimenti divide in due parti se si tratta di visualizzare le shell o di zappare
    int div = 0;
    if(action.substring(0, 3), "shl" || action.substring(0, 3), "zap"){
      div = 2;
    }else{
      div = 3;
    }
    for (int i = 0; i < div; i++) {
      int index = action.indexOf(' ');
      ins_command[i] = action.substring(0, index);
      action = action.substring(index + 1);
    }
  }
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
  reset(6, 0);
  Serial.begin(9600);
}

void loop() {
  checkSerial();

  if(action != ""){
    if(ins_command=="shl"){
        mostra_shell(ins_command[1]);
    } else if(ins_command=="zap"){
          zapping(ins_command[1].toInt());
    }else{
        update_life();
        update_led();
    }
  }
}