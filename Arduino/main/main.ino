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
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    // salvo la stringa intera per eventuale debug
    action = line;

    // split senza modificare 'action'
    String temp = line;
    int idx = 0;
    for (; idx < 3 && temp.length() > 0; idx++) {
      int spaceIndex = temp.indexOf(' ');
      if (spaceIndex == -1) {
        ins_command[idx] = temp;
        temp = "";
      } else {
        ins_command[idx] = temp.substring(0, spaceIndex);
        temp = temp.substring(spaceIndex + 1);
        temp.trim();
      }
    }
    // riempio eventuali slot mancanti
    for (int i = idx; i < 3; i++) ins_command[i] = "";
    // controllo argomenti richiesti
    String cmd = ins_command[0];
    if (cmd == "shl" || cmd == "zap") {
      if (ins_command[1] == "") {
        action = ""; // ignoro il comando
      }
    } else if (cmd == "add" || cmd == "rmv") {
      if (ins_command[1] == "" || ins_command[2] == "") {
        action = ""; // ignoro il comando
      }
    } else {
      action = ""; // ignoro
    }
  }
}

void setup() {
  Serial.begin(9600);
  delay(50);

  setup_servos();

  // impostazione relè e default LOW
  pinMode(data_pin_rele1, OUTPUT); digitalWrite(data_pin_rele1, LOW);
  pinMode(data_pin_rele2, OUTPUT); digitalWrite(data_pin_rele2, LOW);
  pinMode(data_pin_rele3, OUTPUT); digitalWrite(data_pin_rele3, LOW);
  pinMode(data_pin_rele4, OUTPUT); digitalWrite(data_pin_rele4, LOW);

  // inizializzazione FastLED: usa GRB coerente per WS2812B
  FastLED.addLeds<WS2812B, data_pin_g1, GRB>(leds[0], n_led);
  FastLED.addLeds<WS2812B, data_pin_g2, GRB>(leds[1], n_led);
  FastLED.addLeds<WS2812B, data_pin_g3, GRB>(leds[2], n_led);
  FastLED.addLeds<WS2812B, data_pin_g4, GRB>(leds[3], n_led);

  clearLed();

  // inizializzo vite
  for (int i = 0; i < n_player; i++) {
    life[i] = n_led / 2; // es. 6 vite (dato che ogni vita = 2 led)
  }

  reset(6, 0); // mostra inizialmente 6 vite per tutti
}

void loop() {
  checkSerial();

  if(action != ""){
    if(ins_command[0]=="shl"){
        mostra_shell(ins_command[1]);
    } else if(ins_command[0]=="zap"){
          zapping(ins_command[1].toInt());
    }else{
        update_life(ins_command);
        update_led();
    }
  }
}