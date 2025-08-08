#include <FastLED.h>

#define time_zapping 2000
#define n_player 4
#define n_led 12

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
void update_life(){
    String cmd = ins_command[0];
    lifes_ins = ins_command[1].toInt(); 
    player = ins_command[2].toInt();
    player_matrix = player -1;

    if (cmd == "rmv") {
      if(life[player_matrix]==0){
        life[player_matrix] -= lifes_ins;
      }
      zapping(player);
    } else if (cmd == "add" && (life[player_matrix] + lifes_ins) <= n_led) {
      life[player_matrix] += lifes_ins;
    }
    action = "";
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
    fill_solid(leds[j], n_led, CRGB::Black);
  }
  FastLED.show();
}


//resettta i led del giocatore passato come parametro al numero di vite passate
//se il valore passato per il giocatore è 0 imposta le vite di tutti i giocatori
void reset(int n_vite, int player){
  clearLed();
  if(n_vite <=6){
    n_vite*=2;
    if(player==0){
      for(int i=0; i<n_player; i++){
        fill_solid(leds[i], n_vite, CRGB::Green);
      }
    }else{
      fill_solid(leds[player], n_vite, CRGB::Green);
    }
    FastLED.show();
  } else{
    return;
  }
}