#include <Arduino.h>

#define PIN_TEST 16

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World!");

  pinMode(PIN_TEST, OUTPUT);
}

void loop() {
  Serial.println("bonjour");
  printf("TX: %d, RX: %d\r\n", TX, RX);
  digitalWrite(PIN_TEST, HIGH);
  delay(1000);
  digitalWrite(PIN_TEST, LOW);
  delay(1000);
}