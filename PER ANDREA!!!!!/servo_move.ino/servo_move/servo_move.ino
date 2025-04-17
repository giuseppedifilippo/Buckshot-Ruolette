/*
Wiring:
  Black -> Ground
  Red -> 5V
  Yellow -> pin data
*/

#include <Servo.h>

Servo myservo;
int pos1 = 0;
int pos2 = 120;
int pos3 = 240; 
int pinServo = 3;

void setup() {
  myservo.attach(pinServo);
}

void loop() {
  myservo.write(pos1);
  delay(100);
  myservo.write(pos2);
  delay(100);
  myservo.write(pos3);
  delay(100);
}
