/*
// method 1
void setup() {
  pinMode(13, OUTPUT);  // this is CRUCIAL
  GPIOC_PSOR = (1<<5);  // alt: CORE_PIN13_PORTSET = CORE_PIN13_BITMASK;
}

void loop() {
  GPIOC_PTOR = (1<<5);  // setting a 1 makes the bit inverse, K20P64M72SF1RM manual pg 1336
  delay(1000);  // wait for a second
}
*/

// method 2
void setup() {
  pinMode(12, OUTPUT);  // this is CRUCIAL
}

void loop() {
  // this works, C<<7 is pin 12
  GPIOC_PSOR = (1<<7);  // setting a 1 make it high
  delay(1000);  // wait for a second
  GPIOC_PCOR = (1<<7);  // setting a 1 makes the bit clear
  delay(1000);
}

/*
void loop() {
  digitalWriteFast(12, HIGH);
  delay(1000);  // wait for a second
  digitalWriteFast(12, LOW);
  delay(1000);
}
*/
