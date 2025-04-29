String action = "";
int pin = 16;
void checkSerial(){
  if (Serial.available()) {
    action = Serial.readStringUntil('\n');
    action.trim();

    // divide the instruction in 3 parts
    Serial.println(action);
    }
  }



void setup() {
  Serial.begin(115200);
  pinMode(pin, INPUT_PULLUP);
  
}

void loop() {
  checkSerial();
  if (digitalRead(pin) == LOW) {
    Serial.println("negro");
  }
  delay(100);
}