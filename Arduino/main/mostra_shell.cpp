#include <Wire.h>
#include <FastLED.h>
#include "mostra_shell.h"

//14 potenza 7 frequenza
CRGB shellled[32];

void mostra_shell(String shell) {
  int len = shell.length();
  for (int i = 0; i < len && i < 4; i++) {
    if (shell[i] == '0') {
        fill_solid(&shellled[i*4], 4, CRGB::Blue);// esempio: aperto
    } else if (shell[i] == '1') {
       fill_solid(&shellled[i*4], 4, CRGB::Red);// esempio: chiuso
    } else {
      fill_solid(&shellled[i*4], 4, CRGB::Black);// posizione di sicurezza
    }
    FastLED.show();
  }
  delay(2000);
  fill_solid(shellled, 32, CRGB::Black);
  FastLED.show();
} 
