# Autonomous Line-Following Robot

An autonomous line-following robot designed and built from scratch using custom SolidWorks 3D CAD, a custom KiCad Rev A PCB, an 8-channel IR sensor array, and closed-loop PID control on an Arduino Nano.

Designed for **20 to 30 minute sessions** to balance steady progress alongside exams and coursework.

---

## The Route (Step-by-Step Approach)

To keep the project clean, focused, and beginner-friendly, we are building and documenting the robot step by step:

1. **Part A — Foundations & Spec Sheet:** Understand sense-decide-act fundamentals, electronic basics, and measure all hardware in hand.
2. **Part B — Bench Bring-up:** Wire and validate sensors, motors, auto-calibration, and PID on a breadboard without building the chassis yet.
3. **Part C — Chassis from Zero:** Model the chassis, brackets, and sensor mount in SolidWorks; test-fit 3D prints.
4. **Part D — Custom PCB (Rev A):** Design the schematic and layout in KiCad using modular headers, verify 1:1 paper prints, and assemble the board.
5. **Part E — Build, Firmware & Tuning:** Full robot assembly, state-machine race firmware upload, and closed-loop track tuning.
6. **Part F — Competition & Upgrades:** Race-day setup, track element handling (intersections, 90° turns, gaps), and advanced upgrades.

> 📖 **Full Engineering Book:** For the complete in-depth guide with physics derivations, KiCad walkthroughs, SolidWorks lessons, and control theory simulation, see [The Line Follower Book](docs/the_line_follower_book.md).

---

## Component Inventory & Bill of Materials

Here is our complete hardware inventory and procurement checklist:

| No. | Component | Qty | Status | Notes |
| :---: | --- | :---: | :---: | --- |
| 1 | **N20 motors with wheels** | 2 | HAVE | Read the label for voltage and RPM; measure shaft and wheel diameter |
| 2 | **Rubber grip tyres** | 1 set | HAVE | Measure the diameter with the tyres fitted on the wheels |
| 3 | **N20 motor brackets** | 2 | BUY (local store) | The chassis CAD will be drawn around their mounting hole pattern |
| 4 | **RLS-08 8-sensor array** | 1 | HAVE | 8 TCRT5000 IR reflectance pairs; optimal height 3–5 mm above floor (analog pins A0–A7) |
| 5 | **Arduino Nano** | 1 | HAVE | ATmega328P @ 16 MHz, 5V logic; test bootloader with Blink example (CH340 driver if clone) |
| 6 | **TB6612FNG motor driver** | 1 | HAVE | Dual H-bridge driver with STBY enable pin; low voltage drop and minimal heat |
| 7 | **Single LiPo cells** | 2 | HAVE | 3.7 V nominal each (7.4 V in series); measure voltages before joining (must match within 0.05 V) |
| 8 | **LiPo charger** | 1 | HAVE | Charge each cell individually to 4.20 V; never leave charging cells unattended |
| 9 | **Multimeter** | 1 | HAVE | Essential for setting buck converter to 5.0 V, battery monitoring, and continuity checks |
| 10 | **Soldering iron, solder, flux** | 1 set | HAVE | For wire harnesses, power switch, capacitors, and assembling the custom PCB |
| 11 | **Screwdriver set** | 1 | HAVE | For chassis assembly, motor mounting, and adjusting buck converter potentiometer |
| 12 | **USB cable for the Nano** | 1 | HAVE | Mini-USB cable for firmware upload and real-time serial telemetry |
| 13 | **Jumper wires (M-M and M-F)** | 1 set | HAVE | For Part B breadboard prototyping and wiring sensor/driver modules |
| 14 | **Breadboard** | 1 | HAVE | For bench bring-up tests to prove circuits before designing the PCB |
| 15 | **LM2596 buck module (adjustable, no display)** | 1 | BUY (local store) | Set to exactly 5.0 V with multimeter before connecting the Nano 5V rail |
| 16 | **Slide switch (at least 1 A)** | 1 | BUY (local store) | Moves from the breadboard to the PCB later; switches battery positive |
| 17 | **Tactile push button** | 1 | BUY (local store) | The robot needs 2 (Start and Calibrate). You have 1, so buy 1 more |
| 18 | **Capacitors: 100 µF 16 V ×2, 0.1 µF ceramic ×6** | 1 set | BUY (local store) | Motor supply decoupling (100 µF + 0.1 µF across VM/GND) and 5V rail filtering |
| 19 | **LEDs ×2 (one is a backup) & 330 Ω resistors ×2** | 1 set | BUY (local store) | External status LED on D12 with current-limiting resistor |
| 20 | **LiPo battery holder (holds both cells in series, two output wires)** | 1 | BUY (local store) | Take a cell with you to the store and check that it fits |
| 21 | **Battery connector pair (XT30 or JST-XH)** | 1 pair | BUY (local store) | One half on the holder’s wires, the other half on the breadboard lead and later the PCB |
| 22 | **Ball caster** | 1 | BUY (before CAD) | Front low-friction skid/support; its height decides chassis geometry, so measure once in hand |
| 23 | **Wire cutter and stripper** | 1 | BUY (online order) | Essential for making clean harnesses, stripping leads, and wire routing |

---

### Procurement Summary
- **In Hand (14 items):** Motors, tyres, RLS-08 sensor, Nano, TB6612FNG, LiPos, charger, multimeter, soldering kit, tools, USB cable, jumpers, breadboard.
- **To Procure (9 items):** N20 brackets, LM2596 buck module, slide switch, 1 tactile push button, capacitor pack, LEDs + 330 Ω resistors, 2S LiPo holder, battery connector pair, ball caster, wire stripper.

---

## Current Step: Part A (Exam Mode Checklist)

While preparing for exams, work is limited to reading and measuring—no buying, CAD, or soldering required yet:

1. **Inspect Your Components:**
   - Note the rated voltage and RPM on your N20 motors.
   - Count and check the pins on your RLS-08 sensor board.
   - Measure battery voltages using your multimeter (ensure both cells are healthy, $\ge 3.7\text{ V}$).
2. **Fill Your Spec Sheet:**
   - Measure physical dimensions (wheel diameter with rubber tyres fitted, sensor pitch, mounting hole distances).
3. **Set Up Development Software:**
   - Install the **Arduino IDE**.
   - Verify communication with your Arduino Nano (install CH340 driver if needed and test with the Blink example).

---

## Repository Structure

```
Line-Following-Robot/
├── docs/
│   └── the_line_follower_book.md   # Complete step-by-step 40-page engineering guide
└── README.md                       # Project overview, roadmap & bill of materials
```

---

## Reference Pin Mapping

For easy reference during Part B bench wiring:

| Function | Pin | Notes |
| --- | --- | --- |
| **Sensors S1 to S8** | `A0` to `A7` | Left to right analog reflectance signals |
| **Left Motor (PWMA, AIN1, AIN2)** | `D5`, `D7`, `D8` | Speed & direction |
| **Right Motor (PWMB, BIN1, BIN2)** | `D6`, `D4`, `D9` | Speed & direction |
| **Motor Driver Standby (STBY)** | `D3` | Set `HIGH` in code to enable motors |
| **Start / Stop Button** | `D2` | Connect to GND (`INPUT_PULLUP`) |
| **Calibrate Button** | `D10` | Connect to GND (`INPUT_PULLUP`) |
| **Status LED** | `D12` | Driven through 330 Ω resistor to GND |
