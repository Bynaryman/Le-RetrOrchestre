void setup() {
 for (unsigned int i = 8 ; i < 10 ; ++i) {
     pinMode(i << 1, OUTPUT);
     pinMode((i << 1) + 1, OUTPUT);
 }
 for (byte i = 9 ; i < 10 ; ++i) {
      digitalWriteFast((i << 1) + 1, LOW);
      for (int j = 0 ; j < 10000 ; j++) {
        digitalWriteFast(i << 1, HIGH);
        delay(1);
        digitalWriteFast(i << 1, LOW);
        delay(1);
      }
      digitalWriteFast((i << 1) + 1, HIGH);
  }
}

void loop() {}
