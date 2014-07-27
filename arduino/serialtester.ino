int lastpin = 0;
int pinstate = 0;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(19200);
}

void loop() {
  if(millis() - lastpin > 250) {
    if(pinstate == 1) {
      digitalWrite(13, LOW);
      pinstate = 0;
    }
    else {
      digitalWrite(13, HIGH);
      pinstate = 1;
    }
    
    lastpin = millis();
  }
  
  int nbars = 64;
  int inc = 1000 / nbars;
  for(int i = 0; i < nbars; i++) {
    double v = sin((millis() + i * inc)/ 150.0);
    int x = (int) (((v + 1.0) / 2.0) * 255.0);
    Serial.write(x);
  }
  delay(8);
}
