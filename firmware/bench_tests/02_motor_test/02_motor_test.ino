/*
 * Bench Test 2: TB6612FNG & N20 Motor Direction & Dead-Zone Test (Chapter 6)
 * Measures direction and finds the minimum PWM required to spin the motor.
 */

#include <Arduino.h>

const uint8_t PWMA = 5, AIN1 = 7, AIN2 = 8;
const uint8_t PWMB = 6, BIN1 = 4, BIN2 = 9;
const uint8_t STBY = 3;
const int MAX_PWM = 180; // Safety cap (~70%)

void setMotor(uint8_t pwmPin, uint8_t in1, uint8_t in2, int speed) {
  speed = constrain(speed, -MAX_PWM, MAX_PWM);
  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pwmPin, speed);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pwmPin, -speed);
  } else {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
    analogWrite(pwmPin, 0);
  }
}

void drive(int left, int right) {
  setMotor(PWMA, AIN1, AIN2, left);
  setMotor(PWMB, BIN1, BIN2, right);
}

void setup() {
  Serial.begin(115200);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  Serial.println(F("Motor Bench Test Initialized."));
  Serial.println(F("Testing Forward 100 for 1s..."));
  drive(100, 100);
  delay(1000);
  drive(0, 0);
  delay(500);

  Serial.println(F("Testing Reverse 100 for 1s..."));
  drive(-100, -100);
  delay(1000);
  drive(0, 0);
  delay(500);

  Serial.println(F("Starting Dead-zone ramp test (0 to 120 in steps of 5)..."));
  for (int p = 0; p <= 120; p += 5) {
    Serial.print(F("PWM: "));
    Serial.println(p);
    drive(p, p);
    delay(400);
  }
  drive(0, 0);
  Serial.println(F("Test Complete. Note the PWM value where wheels began spinning."));
}

void loop() {
  // Idle
}
