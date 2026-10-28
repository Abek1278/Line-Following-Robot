/*
 * Bench Test 3: RLS-08 Calibration & Weighted Position Test (Chapter 8)
 * Calibrates min/max reflectances over 4 seconds and outputs normalized position (0 to 7000).
 */

#include <Arduino.h>

const uint8_t NUM = 8;
const uint8_t sensorPins[NUM] = {A0, A1, A2, A3, A4, A5, A6, A7};
const bool RAW_DROPS_ON_LINE = false;

uint16_t minV[NUM];
uint16_t maxV[NUM];
int lastPos = 3500;

void calibrate(unsigned long ms) {
  for (uint8_t i = 0; i < NUM; i++) {
    minV[i] = 1023;
    maxV[i] = 0;
  }
  unsigned long start = millis();
  while (millis() - start < ms) {
    for (uint8_t i = 0; i < NUM; i++) {
      uint16_t v = analogRead(sensorPins[i]);
      if (v < minV[i]) minV[i] = v;
      if (v > maxV[i]) maxV[i] = v;
    }
  }
}

int readPosition() {
  long weighted = 0, sum = 0;
  for (uint8_t i = 0; i < NUM; i++) {
    long range = (long)maxV[i] - minV[i];
    long n = range > 0 ? ((long)analogRead(sensorPins[i]) - minV[i]) * 1000L / range : 0;
    n = constrain(n, 0, 1000);
    if (RAW_DROPS_ON_LINE) n = 1000 - n;
    if (n > 100) {
      weighted += n * i * 1000L;
      sum += n;
    }
  }
  if (sum < 200) return (lastPos < 3500) ? 0 : 7000;
  lastPos = weighted / sum;
  return lastPos;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  Serial.println(F("Starting 4-second calibration... Sweep sensor across black line and white surface now!"));
  calibrate(4000);
  Serial.println(F("Calibration Complete."));

  for (uint8_t i = 0; i < NUM; i++) {
    Serial.print(F("S"));
    Serial.print(i + 1);
    Serial.print(F(": Min="));
    Serial.print(minV[i]);
    Serial.print(F(", Max="));
    Serial.print(maxV[i]);
    Serial.print(F(" | "));
  }
  Serial.println();
}

void loop() {
  int pos = readPosition();
  Serial.print(F("Position (0-7000, 3500=center): "));
  Serial.println(pos);
  delay(100);
}
