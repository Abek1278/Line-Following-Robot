# Autonomous Line-Following Robot: From Zero to Competition

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Arduino%20Nano-teal.svg)](https://www.arduino.cc/)
[![CAD](https://img.shields.io/badge/CAD-SolidWorks-red.svg)](cad/)
[![PCB](https://img.shields.io/badge/EDA-KiCad-orange.svg)](pcb/)

A complete, ground-up autonomous line-following robot built for high speed and competitive reliability. Designed around an **Arduino Nano**, **RLS-08 8-channel IR sensor array**, **TB6612FNG dual H-bridge**, **N20 micro metal gear motors**, and a **custom 3D-printed chassis & PCB**.

---

## Table of Contents
- [Project Overview](#project-overview)
- [System Architecture](#system-architecture)
- [Hardware Bill of Materials](#hardware-bill-of-materials)
- [Pinout Mapping](#pinout-mapping)
- [Power & Battery Management](#power--battery-management)
- [Control Theory & Algorithm](#control-theory--algorithm)
- [Project Directory Structure](#project-directory-structure)
- [Getting Started](#getting-started)
- [Documentation & The Book](#documentation--the-book)

---

## Project Overview

This project bypasses throwaway cardboard/perfboard prototypes in favor of an iterative engineering workflow:
1. **Part A: Foundations & Specifications:** Hardware measurement, electronic fundamentals, and physics calculations.
2. **Part B: Bench Bring-up:** Independent breadboard validation of sensors, motor dead-zones, calibration, and loop timing.
3. **Part C: Custom Chassis:** Parametric 3D CAD design in SolidWorks with test-fit prints.
4. **Part D: Custom PCB (Rev A):** Modular KiCad schematic & layout with star ground planes, decoupling, and header sockets.
5. **Part E: Race Firmware & PID Tuning:** Deterministic 1 kHz closed-loop control with dynamic corner slowdown.
6. **Part F: Competition:** Race-day calibration protocols, track elements handling, and upgrades.

---

## System Architecture

```
   +--------------------------------------------------------------+
   |                       2S LiPo Pack (7.4V - 8.4V)             |
   +------------------------------+-------------------------------+
                                  |
                                  +--> [Power Switch]
                                            |
                         +------------------+------------------+
                         |                                     |
                         v                                     v
                 [TB6612FNG (VM)]                    [5V Buck Converter]
                         |                                     |
                         v                                     +--------------------+
                  [N20 Motors]                                 |                    |
                                                               v                    v
                                                        [Arduino Nano]       [RLS-08 Array]
                                                               |
                                                 +-------------+-------------+
                                                 | (PWM / Direction Signals) |
                                                 v                           v
                                            Left Motor                  Right Motor
```

- **Sense:** 8 TCRT5000 IR reflectance pairs sampled via analog inputs $A0$ to $A7$.
- **Decide:** ATmega328P microcontroller computes weighted centroid position and executes PID closed-loop steering corrections at a steady **1 kHz ($1000\ \mu\text{s}$)** cycle time.
- **Act:** TB6612FNG dual MOSFET driver modulates PWM differential speeds to two high-RPM N20 geared motors.
- **Power:** 2-cell series LiPo (7.4V nominal) directly drives the motors via TB6612 VM; a buck converter steps down the supply to an exact $5.0\text{V}$ for the logic rails.

---

## Hardware Bill of Materials

| Component | Function | Specifications / Notes |
| --- | --- | --- |
| **Arduino Nano** | Main Controller | ATmega328P @ 16 MHz, 5V logic, 8 analog inputs |
| **RLS-08 Array** | Line Detection | 8 TCRT5000 IR sensor pairs, calibrated height 3–5 mm |
| **TB6612FNG** | Motor Driver | Dual H-bridge, 1.2A continuous / 3.2A peak, low $V_{drop}$ |
| **N20 Motors** | Drive Actuators | 6V micro metal gear DC motors with matching wheels |
| **5V Buck Module** | Logic Regulator | Steps 7.4V–8.4V battery voltage down to clean 5.0V |
| **LiPo Cells (×2)**| Main Energy Source | 3.7V single cells in series configuration (~7.4V) |
| **Decoupling Caps**| Noise Suppression | $100\ \mu\text{F}$ electrolytic + $0.1\ \mu\text{F}$ ceramic across VM & GND |
| **Tactile Buttons**| UI Input | Start/Stop (D2) and Auto-Calibration (D10) |
| **Status LED** | UI Feedback | D12 with 330 $\Omega$ current-limiting resistor |

---

## Pinout Mapping

All pin assignments are shared across the bench bring-up and custom KiCad Rev A PCB:

| Subsystem | Signal Name | Nano Pin | Notes |
| --- | --- | --- | --- |
| **RLS-08 Sensors** | S1 to S8 (Left $\rightarrow$ Right) | `A0` to `A7` | Analog reflectance signals (0–1023) |
| **Left Motor** | PWMA | `D5` | Hardware PWM speed control |
| | AIN1 | `D7` | Direction control 1 |
| | AIN2 | `D8` | Direction control 2 |
| **Right Motor** | PWMB | `D6` | Hardware PWM speed control |
| | BIN1 | `D4` | Direction control 1 |
| | BIN2 | `D9` | Direction control 2 |
| **Motor Driver Enable** | STBY | `D3` | Must be driven `HIGH` to enable H-bridges |
| **Push Buttons** | Start / Stop | `D2` | Configured as `INPUT_PULLUP` (to GND) |
| | Calibrate | `D10` | Configured as `INPUT_PULLUP` (to GND) |
| **Status LED** | External LED | `D12` | Driven through 330 $\Omega$ resistor |
| **Auxiliary / Spare**| Bluetooth HC-05 | `D0` (RX), `D1` (TX) | Optional wireless telemetry; disconnect during upload |

---

## Power & Battery Management

> [!CAUTION]
> **LiPo Safety Rules:**
> 1. Charge each 1S cell individually using a dedicated LiPo charger.
> 2. Measure both cells with a multimeter before connecting in series; cell voltages must match within **0.05V**.
> 3. Adjust the buck regulator to exactly **5.00V** on the bench *before* connecting the Arduino Nano.
> 4. Never let cell voltages drop below **3.60V** resting voltage.
> 5. Always turn the robot power switch **OFF** before plugging the Arduino Nano into a computer via USB.

---

## Control Theory & Algorithm

### 1. Weighted Average Centroid
Each calibrated sensor $i \in [0, 7]$ reading is normalized from 0 to 1000. Position is computed continuously:
$$\text{position} = \frac{\sum_{i=0}^{7} (\text{norm}_i \times i \times 1000)}{\sum_{i=0}^{7} \text{norm}_i}$$
- **Centre Position:** $3500$
- **Total Range:** $0$ (far left) to $7000$ (far right)
- **Line Lost Recovery:** If $\sum \text{norm}_i < 200$, the robot steers towards the last recorded edge until recovered or timed out.

### 2. PID Steering Equation
$$\text{error} = \text{position} - 3500$$
$$\text{correction} = K_p \times \text{error} + K_i \int \text{error}\,dt + K_d \frac{d(\text{error})}{dt}$$

### 3. Dynamic Corner Slowdown
$$\text{effectiveSpeed} = \text{constrain}\left(\text{maxSpeed} - \frac{|\text{error}|}{\text{slowDiv}}, \text{minSpeed}, \text{maxSpeed}\right)$$
$$\text{leftPWM} = \text{effectiveSpeed} + \text{correction}$$
$$\text{rightPWM} = \text{effectiveSpeed} - \text{correction}$$

---

## Project Directory Structure

```
Line-Following-Robot/
├── .github/                       # CI workflows & issue templates
├── cad/                           # SolidWorks models & 3D print specs
│   └── README.md                  # Chassis layout, print settings & tolerances
├── docs/                          # Guides, specifications, and logs
│   ├── spec_sheet.md              # Hardware measurement spec sheet
│   ├── the_line_follower_book.md  # Complete step-by-step guidebook
│   └── tuning_log.md              # Session & PID tuning log template
├── firmware/                      # Arduino code
│   ├── bench_tests/               # Hardware verification sketches
│   │   ├── 01_sensor_test/        # Analog read timing & finger test
│   │   ├── 02_motor_test/         # Direction & dead-zone ramp test
│   │   └── 03_calibration_test/   # Calibration & position test
│   └── line_follower/             # Production race firmware
│       └── line_follower.ino      # 4-state FSM + 1 kHz PID loop
├── pcb/                           # KiCad schematics & PCB layouts
│   └── README.md                  # KiCad Rev A rules, stackup & checklist
├── simulation/                    # Control simulation tools
│   └── pid_simulation.py          # Python PID response & settling simulation
├── LICENSE                        # MIT License
└── README.md                      # Project documentation
```

---

## Getting Started

### Bench Bring-up
1. Connect the RLS-08 sensor to `A0`-`A7` on an Arduino Nano. Upload [`01_sensor_test.ino`](firmware/bench_tests/01_sensor_test/01_sensor_test.ino) and perform the finger test to verify array indexing.
2. Connect TB6612FNG and motors on a breadboard. Upload [`02_motor_test.ino`](firmware/bench_tests/02_motor_test/02_motor_test.ino) to determine motor forward direction and starting dead-zone PWM.
3. Upload [`03_calibration_test.ino`](firmware/bench_tests/03_calibration_test/03_calibration_test.ino) to test sensor calibration across black tape and white background.

### Race Firmware Upload
Upload [`line_follower.ino`](firmware/line_follower/line_follower.ino):
- **Press Calibrate Button (`D10`):** Status LED turns on. Sweep robot over line for 4 seconds.
- **Press Start Button (`D2`):** 1-second countdown delay begins, after which the robot runs the 1 kHz PID control loop.
- **Press Start Button again:** Emergency stop returning robot to `READY` state.

---

## Documentation & The Book

For complete step-by-step instructions across all 6 phases:
- **Full Guidebook:** [The Line Follower Book](docs/the_line_follower_book.md)
- **Component Measurement Sheet:** [Spec Sheet](docs/spec_sheet.md)
- **Tuning & Session Log:** [Tuning Log](docs/tuning_log.md)
- **Chassis CAD Guide:** [CAD Readme](cad/README.md)
- **PCB KiCad Guide:** [PCB Readme](pcb/README.md)

---

## License

This project is licensed under the [MIT License](LICENSE).
