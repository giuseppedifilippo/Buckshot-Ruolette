#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Creo il driver PCA9685 con indirizzo I²C predefinito 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Definizione min e max in tick per il segnale dei servo
#define SERVOMIN  150 // impulso minimo (~0°)
#define SERVOMAX  600 // impulso massimo (~180°)

// Canali PCA9685 dove sono collegati i servo
int servoChannels[4] = {0, 1, 2, 3/*, 4, 5, 6, 7*/}; // 0-15 disponibili

// Funzione per mappare gradi → tick PCA9685
void setServoAngle(uint8_t channel, int angle) {
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulse);
}

void setup_servos() {
  pwm.begin();
  pwm.setPWMFreq(50); // 50 Hz per i servo standard
  delay(10);

  // Porta tutti i servo a 0°
  for (int i = 0; i < 4; i++) {
    setServoAngle(servoChannels[i], 0);
  }
}

void mostra_shell(String shell) {
  int len = shell.length();
  for (int i = 0; i < len && i < 4; i++) {
    if (shell[i] == '0') {
      setServoAngle(servoChannels[i], 60); // esempio: aperto
    } else if (shell[i] == '1') {
      setServoAngle(servoChannels[i], 120); // esempio: chiuso
    } else {
      setServoAngle(servoChannels[i], 0); // posizione di sicurezza
    }
  }
  delay(2000);

  // Riporta tutti a 0°
  for (int i = 0; i < 4; i++) {
    setServoAngle(servoChannels[i], 0);
  }
}
