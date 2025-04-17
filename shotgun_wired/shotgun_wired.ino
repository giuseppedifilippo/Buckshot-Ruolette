//versione del programma del fucile ma fatto per funzionare via cavo

#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu{ Wire };
int trigger_pin = 15;    //pin su cui è collegato l'interruttore del grilletto
int light = 34;          //pin per i led che simulano il flash del fucile
int recalibration = 35;  //pin collegato a un interrupt che avvia la ricalibrazione del mpu6050
byte aiming;


void Recalibrate() {
  mpu.calcOffsets();
}


void setup() {
  Serial.begin(115200);
  pinMode(trigger_pin, INPUT_PULLDOWN);
  pinMode(light, OUTPUT);
  pinMode(recalibration, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(recalibration), Recalibrate, RISING);
  attachInterrupt(digitalPinToInterrupt(trigger_pin), Send, RISING);
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {}  // stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets();  // ricalibra il sensore
  Serial.println("Done!n");
}
unsigned long timer = millis();

//indicazione per i dati che la scheda manda all rpi
//F = mirando di fronte
//S = mirando a se stesso
//L = mirando a sinistra
void loop() {
  mpu.update();
  //magari quest parte si può rifare col task scheduler
  //tutto sto pappone serve a capire in che direzione sta puntando il fucile
  if ((millis() - timer) > 10) {  //aggiorna la posizione ogni 10ms
    if (mpu.getAngleZ() < 20 && mpu.getAngleZ() > -20 && mpu.getAngleX() < 90) {
      aiming = '11';
    } else if (mpu.getAngleZ() < -20 && mpu.getAngleZ() > -60 && mpu.getAngleX() < 90) {
      aiming = '10';
    } else if (mpu.getAngleZ() < 60 && mpu.getAngleZ() > 20 && mpu.getAngleX() < 90) {
      aiming = '01';
    }
    if (mpu.getAngleX() > 90 && mpu.getAngleX() < 180) {
      aiming = '00';
    }
    timer = millis();
  }
}
