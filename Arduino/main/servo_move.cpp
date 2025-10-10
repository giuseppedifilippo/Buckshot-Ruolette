#include <Wire.h>
#include <FastLED.h>

// Canali PCA9685 dove sono collegati i servo
int shellChannels[8] = {0, 1, 2, 3, 4, 5, 6, 7}; 
int shellMatrix[8][4]

void shellSetup(){
  // inizializzazione FastLED: usa GRB coerente per WS2812B
  FastLED.addLeds<WS2812B, data_pin_g1, GRB>(shellChannel[0], 4);
  FastLED.addLeds<WS2812B, data_pin_g2, GRB>(shellChannel[1], 4);
  FastLED.addLeds<WS2812B, data_pin_g3, GRB>(shellChannel[2], 4);
  FastLED.addLeds<WS2812B, data_pin_g4, GRB>(shellChannel[3], 4);
  FastLED.addLeds<WS2812B, data_pin_g4, GRB>(shellChannel[4], 4);
  FastLED.addLeds<WS2812B, data_pin_g4, GRB>(shellChannel[5], 4);
  FastLED.addLeds<WS2812B, data_pin_g4, GRB>(shellChannel[6], 4);
  FastLED.addLeds<WS2812B, data_pin_g4, GRB>(shellChannel[7], 4);
}

void mostra_shell(String shell) {
  int len = shell.length();
  for (int i = 0; i < len && i < 4; i++) {
    if (shell[i] == '0') {
       // esempio: aperto
    } else if (shell[i] == '1') {
       // esempio: chiuso
    } else {
       // posizione di sicurezza
    }
  }
  delay(2000);

  // Riporta tutti a 0°
  for (int i = 0; i < 4; i++) {
   
  }
}
