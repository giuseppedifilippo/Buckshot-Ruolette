#include <Arduino.h>
#include <Servo.h>
#include "servo_move.h"

void move_servo(int pin, int angle) {
  Servo myServo;
  myServo.attach(pin);
  myServo.write(angle);
  delay(1000);
  myServo.detach();
}