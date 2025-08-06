#include <Arduino.h>
#include <Servo.h>
#include "servo_move.h"

// Dichiarazione dell'array di oggetti Servo
Servo servos[8]; 

// Array dei pin a cui sono collegati i servo (da 2 a 9)
int servoPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

void setup_servos(){
  for (int i = 0; i < 8; i++) {
    servos[i].attach(servoPins[i]); // Attacca ogni servo al suo pin
    servos[i].write(0); // Opzionale: imposta tutti i servo a 0 gradi all'avvio
  }
}

void mostra_shell(String shell){
  // Nota: shell.indexOf('\0') non è tipico per String, che non è null-terminated.
  // Se 'shell' è un oggetto String, usa shell.length()
  // Se 'shell' è un char[], allora indexOf('\0') ha senso.
  // Assumo che 'shell' sia un oggetto String e che tu voglia iterare sulla sua lunghezza.
  int len = shell.length(); 
  for(int i = 0; i < len; i++){
    if(shell[i]=='0'){
      servos[i].write(120);
    }else if(shell[i]=='1'){
      servos[i].write(240);
    }else{ // Se il carattere non è '0' o '1', imposta a 0
      servos[i].write(0);
    }
  }
  delay(2000);
  for(int i = 0; i < 8; i++){ // Assicurati che questo loop sia corretto per il numero di servo
    servos[i].write(0);
  }
}