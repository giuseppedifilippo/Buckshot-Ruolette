#include <FastLED.h>

#define time_zapping 2000
#define n_player 4
#define n_led 8

extern int life[n_player];
extern CRGB leds[n_player][n_led];

extern String action;
extern String ins_command[3]; 

//vita e giocatore indicata dall'istruzione di aggiunta e rimozione
int lifes_ins = 0;
int player = 0;

//numero del giocatore all'interno dell'array di giocatori
int player_matrix = 0;

//invia un segnale ai relè per permettere il passaggio della corrente attraverso gli elettrodi
void zapping(int player){
  player += 6;
  digitalWrite(player, HIGH);
  delay(time_zapping);
  digitalWrite(player, LOW);
}

//aggiorna il numero delle vite quando viene inviato un messaggio in seriale
//quando la vita viene persa richiama la funzione zapping per folgorare il giocatore
//il messaggio è formattato così: istruzione numero_vite numero_giocatore. Le istruzioni possono essere "rmv" per rimuovere e "add" per aggiungere
void update_life(String ins_cmd[]){
  String cmd = ins_cmd[0];
  lifes_ins = ins_cmd[1].toInt();
  player = ins_cmd[2].toInt();
  player_matrix = player - 1;

  if (player_matrix < 0 || player_matrix >= n_player) {
    Serial.println("update_life: player fuori range");
    action = "";
    return;
  }

  if (cmd == "rmv" || cmd == "rm") {
    // sottrai senza andare sotto 0
    life[player_matrix] = max(0, life[player_matrix] - lifes_ins);
    Serial.print("Rimosse ");
    Serial.print(lifes_ins);
    Serial.print(" vite a player ");
    Serial.println(player);
    zapping(player); // folgora il giocatore
  } else if (cmd == "add") {
    // non superare n_led/2 (numero max vite)
    int maxLives = n_led / 2;
    life[player_matrix] = min(maxLives, life[player_matrix] + lifes_ins);
    Serial.print("Aggiunte ");
    Serial.print(lifes_ins);
    Serial.print(" vite a player ");
    Serial.println(player);
  } else {
    Serial.print("Comando update_life sconosciuto: ");
    Serial.println(cmd);
  }
  action = "";
}

//accende un numero di led corrispondente al numero di vite (N.B. ogni vita è indicata da due led)
void update_led(){
  if (player_matrix < 0 || player_matrix >= n_player) return;
  // ogni vita = 2 LED
  int ledsOn = life[player_matrix] * 2;
  for (int i = 0; i < n_led; i++) {
    if (i < ledsOn) leds[player_matrix][i] = CRGB::Green;
    else leds[player_matrix][i] = CRGB::Black;
  }
  FastLED.show();
}

//resetta il numero di led accesi spegnendoli tutti
void clearLed(){
  for(int j=0; j<n_player; j++){
    fill_solid(leds[j], n_led, CRGB::Black);
  }
  FastLED.show();
}


//resettta i led del giocatore passato come parametro al numero di vite passate
//se il valore passato per il giocatore è 0 imposta le vite di tutti i giocatori
void reset(int n_vite, int playerIndex){
  clearLed();
  if (n_vite <= 6 && n_vite >= 0) {
    int n_led_to_set = n_vite * 2;
    if (playerIndex == 0) {
      for (int i = 0; i < n_player; i++) {
        fill_solid(leds[i], n_led_to_set, CRGB::Green);
      }
    } else if (playerIndex > 0 && playerIndex <= n_player) {
      fill_solid(leds[playerIndex - 1], n_led_to_set, CRGB::Green);
    }
    FastLED.show();
  }
}