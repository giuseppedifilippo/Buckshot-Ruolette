#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define MinImpulse 150
#define MaxImpulse 600

int servochannel[8] = {0, 1, 2, 3, 4, 5, 6, 7};

void setServoAngle(uint8_t channel, int angle){
  int pulse = map(angle, 0, 180, MinImpulse, MaxImpulse);
  pwm.setPWM(channel, 0, pulse);
}

void setupServo(){
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(10);

  for(int i=0; i<8; i++){
    setServoAngle(servochannel[i], 0);
  }
}

void setup() {
  setupServo();
}

void loop() {
  for(int i=0; i<8; i++){
    setServoAngle(servochannel[i], 60);
    delay(500);
  }
  delay(1000);
  for(int i=0; i<8; i++){
    setServoAngle(servochannel[i], 0);
  }
  delay(1000);
}
