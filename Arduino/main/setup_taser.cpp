#include <Arduino.h>

extern int pin1_taser;
extern int pin2_taser;
extern int pin3_taser;

void setup_taser(int pin1_taser, int pin2_taser, int pin3_taser){
  int del=50;
  digitalWrite(pin1_taser, 1);
  delay(del);
  digitalWrite(pin1_taser, 0);
  
  for(int j=0; j<7; j++){
    digitalWrite(pin2_taser, 1);
    delay(del);
    digitalWrite(pin2_taser, 0); 
    delay(del);
  }

  for(int j=0; j<14; j++){
    digitalWrite(pin3_taser, 1);
    delay(del);
    digitalWrite(pin3_taser, 0); 
    delay(del);
  }
}
