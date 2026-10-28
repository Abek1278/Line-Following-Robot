/*
 * Bench Test 1: RLS-08 Sensor Read & Finger Test (Chapter 5)
 * Reads all 8 IR sensors and prints their raw values + read duration.
 */

#include <Arduino.h>

const uint8_t NUM = 8;
const uint8_t sensorPins[NUM] = {A0, A1, A2, A3, A4, A5, A6, A7};

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println(F("RLS-08 Sensor Array Test"));
  Serial.println(F("S1\tS2\tS3\tS4\tS5\tS6\tS7\tS8\t| Time(us)"));
}

void loop() {
  unsigned long t0 = micros();
  uint16_t raw[NUM];
  for (uint8_t i = 0; i < NUM; i++) {
    raw[i] = analogRead(sensorPins[i]);
  }
  unsigned long readTime = micros() - t0;

  for (uint8_t i = 0; i < NUM; i++) {
    Serial.print(raw[i]);
    Serial.print('\t');
  }
  Serial.print(F("| "));
  Serial.println(readTime);
  delay(100);
}
