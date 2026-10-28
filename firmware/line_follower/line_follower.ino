/*
 * ==============================================================================
 * Project: Autonomous Line Following Robot
 * Architecture: Arduino Nano, TB6612FNG, RLS-08 (8 IR array), 2S LiPo + 5V Buck
 * Description: Closed-loop PID controller with dynamic corner slowdown,
 *              line-lost edge memory, auto-calibration, and 4-state FSM.
 * ==============================================================================
 */

#include <Arduino.h>

// -----------------------------------------------------------------------------
// Pin Configuration (Matches KiCad Rev A & Pin Map Appendix A)
// -----------------------------------------------------------------------------
const uint8_t NUM_SENSORS = 8;
const uint8_t SENSOR_PINS[NUM_SENSORS] = {A0, A1, A2, A3, A4, A5, A6, A7};

// TB6612FNG Motor Driver Pins
const uint8_t PWMA = 5;   // Left Motor PWM
const uint8_t AIN1 = 7;   // Left Motor Direction 1
const uint8_t AIN2 = 8;   // Left Motor Direction 2
const uint8_t PWMB = 6;   // Right Motor PWM
const uint8_t BIN1 = 4;   // Right Motor Direction 1
const uint8_t BIN2 = 9;   // Right Motor Direction 2
const uint8_t STBY = 3;   // Driver Standby Enable (Active HIGH)

// UI & Control Pins
const uint8_t BTN_START = 2;    // Start/Stop Toggle (Active LOW)
const uint8_t BTN_CAL   = 10;   // Calibration Trigger (Active LOW)
const uint8_t LED_STATUS = 12;  // External Status LED (D13 is Nano onboard)

// -----------------------------------------------------------------------------
// Tuning Parameters & Constants
// -----------------------------------------------------------------------------
// Set to true if sensor reading drops over the black line (verify in bench test)
const bool RAW_DROPS_ON_LINE = false;

// Speed limits: Max PWM capped to ~70% (180/255) for 6V N20 motors on 8.4V pack
const int MAX_PWM   = 180;
const int BASE_PWM  = 100;
const int MIN_PWM   = 50;   // Keep above motor dead-zone
const int SLOW_DIV  = 40;   // Dynamic slowdown factor: lower slows more in sharp turns

// PID Gains (Starting points; tune on track)
float Kp = 0.05;
float Ki = 0.00;
float Kd = 0.50;

// Timing Constants
const unsigned long LOOP_US          = 1000;  // 1 kHz deterministic control loop
const unsigned long LOST_TIMEOUT_MS  = 500;   // Halt motors if line lost for > 500 ms

// -----------------------------------------------------------------------------
// State Machine & Variables
// -----------------------------------------------------------------------------
enum RobotState {
  IDLE,
  CALIBRATING,
  READY,
  RUNNING
};

RobotState currentState = IDLE;

uint16_t minVal[NUM_SENSORS];
uint16_t maxVal[NUM_SENSORS];
int lastPosition = 3500;
int lastError = 0;
long integralSum = 0;
bool isLineLost = false;

unsigned long lastLoopMicros = 0;
unsigned long lineLostTimestamp = 0;

// -----------------------------------------------------------------------------
// Motor Control Routine
// -----------------------------------------------------------------------------
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
    // Active braking mode
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
    analogWrite(pwmPin, 0);
  }
}

void drive(int leftSpeed, int rightSpeed) {
  setMotor(PWMA, AIN1, AIN2, leftSpeed);
  setMotor(PWMB, BIN1, BIN2, rightSpeed);
}

// -----------------------------------------------------------------------------
// Sensor Calibration & Reading
// -----------------------------------------------------------------------------
void calibrateSensors(unsigned long durationMs) {
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    minVal[i] = 1023;
    maxVal[i] = 0;
  }
  
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
      uint16_t v = analogRead(SENSOR_PINS[i]);
      if (v < minVal[i]) minVal[i] = v;
      if (v > maxVal[i]) maxVal[i] = v;
    }
  }
}

int readSensorPosition() {
  long weightedSum = 0;
  long totalSum = 0;
  
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    long range = (long)maxVal[i] - minVal[i];
    long norm = 0;
    if (range > 0) {
      norm = ((long)analogRead(SENSOR_PINS[i]) - minVal[i]) * 1000L / range;
    }
    norm = constrain(norm, 0, 1000);
    if (RAW_DROPS_ON_LINE) {
      norm = 1000 - norm;
    }
    
    // Ignore noise threshold (< 10% reflection intensity)
    if (norm > 100) {
      weightedSum += norm * (long)i * 1000L;
      totalSum += norm;
    }
  }
  
  // Line lost detection
  if (totalSum < 200) {
    isLineLost = true;
    // Turn aggressively toward the side where the line was last detected
    return (lastPosition < 3500) ? 0 : 7000;
  }
  
  isLineLost = false;
  lastPosition = (int)(weightedSum / totalSum);
  return lastPosition;
}

// Debounced button read
bool isButtonPressed(uint8_t pin) {
  if (digitalRead(pin) == LOW) {
    delay(30);
    if (digitalRead(pin) == LOW) {
      while (digitalRead(pin) == LOW) {} // Wait for button release
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
// Control Step (1 kHz execution)
// -----------------------------------------------------------------------------
void executeControlLoop() {
  unsigned long now = micros();
  if (now - lastLoopMicros < LOOP_US) return;
  lastLoopMicros = now;

  int position = readSensorPosition();
  
  // Safe halt if line lost for too long
  if (isLineLost) {
    if (lineLostTimestamp == 0) lineLostTimestamp = millis();
    if (millis() - lineLostTimestamp > LOST_TIMEOUT_MS) {
      drive(0, 0);
      currentState = READY;
      return;
    }
  } else {
    lineLostTimestamp = 0;
  }

  // PID Calculation
  int error = position - 3500;
  integralSum = constrain(integralSum + error, -20000, 20000);
  int derivative = error - lastError;
  lastError = error;

  float correction = Kp * error + Ki * integralSum + Kd * derivative;

  // Dynamic turn speed reduction
  int effectiveSpeed = constrain(MAX_PWM - abs(error) / SLOW_DIV, MIN_PWM, MAX_PWM);
  
  int leftPWM  = effectiveSpeed + (int)correction;
  int rightPWM = effectiveSpeed - (int)correction;

  drive(leftPWM, rightPWM);
}

// -----------------------------------------------------------------------------
// Setup & Main Loop
// -----------------------------------------------------------------------------
void setup() {
  // Optional: speed up ADC clock prescaler for faster analog conversions
  // ADCSRA = (ADCSRA & 0xF8) | 0x04; // Prescaler 16

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH); // Enable H-bridge

  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_CAL, INPUT_PULLUP);
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, LOW);

  drive(0, 0);
}

void loop() {
  switch (currentState) {
    case IDLE:
      digitalWrite(LED_STATUS, LOW);
      if (isButtonPressed(BTN_CAL)) {
        currentState = CALIBRATING;
      }
      break;

    case CALIBRATING:
      digitalWrite(LED_STATUS, HIGH);
      calibrateSensors(4000); // 4-second sweep across track
      digitalWrite(LED_STATUS, LOW);
      currentState = READY;
      break;

    case READY:
      // Blink LED slowly to signal readiness
      digitalWrite(LED_STATUS, (millis() / 500) % 2);
      if (isButtonPressed(BTN_START)) {
        integralSum = 0;
        lastError = 0;
        lineLostTimestamp = 0;
        digitalWrite(LED_STATUS, HIGH);
        delay(1000); // 1-second countdown delay
        currentState = RUNNING;
      }
      break;

    case RUNNING:
      executeControlLoop();
      // Allow stopping mid-run with Start button
      if (isButtonPressed(BTN_START)) {
        drive(0, 0);
        currentState = READY;
      }
      break;
  }
}
