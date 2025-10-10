// servo_move.h
#ifndef SERVO_MOVE_H
#define SERVO_MOVE_H

#include <Arduino.h>
#include <FastLED.h>

// Funzioni
void setup_servos();
void mostra_shell(String shell);
void setServoAngle(uint8_t servo, int angle);

#endif

