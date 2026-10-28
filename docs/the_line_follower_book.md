# The Line Follower Book: From Zero to Competition

*A step-by-step book for a beginner who will design the chassis and the PCB from scratch. Kit: N20 motors, RLS-08 sensor array, Arduino Nano, TB6612FNG, 2 single LiPo cells. Built for 20 to 30 minute sessions around GATE and exams.*

## Start here

You will build a line follower from zero: your own chassis (SolidWorks, 3D printed), your own PCB (KiCad), your own code, then tune it on a real track. There is no throwaway prototype in between, so every step in this book is a step toward the final robot.

### The route

1. **Part A, Foundations:** how the robot works, your parts, the electronics you need.
2. **Part B, Bench bring-up:** prove the sensor, motors, calibration and control ideas using only the Nano and a breadboard.
3. **Part C, Chassis from zero:** design it in SolidWorks, test-print, then print the real one.
4. **Part D, Your own PCB:** learn KiCad, design Rev A, order it, bring it up.
5. **Part E, Build and tune:** assemble, write the race firmware, tune on a real track.
6. **Part F, Competition:** rules, race day, upgrades.

### The honest risks, and what protects you

Going straight to a custom chassis and PCB is a good way to learn, but it is riskier than building a rough prototype first. These are the risks and the protections built into this book.

| Risk | Why it happens | Protection in this book |
| --- | --- | --- |
| First PCB has a mistake | Almost every first board does | Rev A uses plug-in modules (Nano, TB6612, buck), has test points and spare pads, and Part B proves the circuit on a breadboard first |
| Chassis does not fit the parts | Real parts differ from datasheets by a millimetre or two | Measure everything in Part A, print small test pieces first (Part C), print the real chassis last |
| Control code is untested until the robot exists | No moving robot yet | Part B tests every piece on the bench, and a laptop simulation builds your feel for PID before you touch a real robot |
| Cost grows | A PCB order adds to the budget | Appendix C lists spend by stage so you can stop at any point |
| Time runs out | GATE and exams come first | Exam mode below: reading and measuring only |

## The rules of this book

**Exam mode versus build mode**

| Mode | When | What you do | What you do not do |
| --- | --- | --- | --- |
| Exam mode | Until end-sem is over | Measure your parts (Part A), read one chapter a week | No buying, no CAD, no soldering |
| Build mode | After exams | Follow Parts B to F in order | Skip ahead before a chapter's check passes |

**Session size:** 20 to 30 minutes. Stop when time is up, even in the middle of a chapter. Every chapter ends with a **Done when** check so you know where to restart.

**One change at a time, and write it down.** Keep a log (the template is in Appendix F). Robots improve when you know exactly which change helped. They stay stuck when you change three things and cannot tell which one mattered.

**Labels used in this book**

- **Must know / Should know / Nice to know** tell you what to prioritise.
- *Verified* means seen in a datasheet or listing (still check your own board).
- *Rule of thumb* means common practice, not proven on your robot.
- *Starting value* means try it, then adjust.
- *Untested* means I have not been able to run it on your hardware.

**How much work this is:** roughly 60 to 90 sessions of 20 to 30 minutes (rule of thumb), which is about 3 to 5 months at 4 sessions a week.

### Reading map

| Part | After it you can | Sessions | Approx. spend (₹) |
| --- | --- | --- | --- |
| A Foundations | Explain every part of the robot and fill your spec sheet | 5-6 | 0 |
| B Bench bring-up | Read the sensor, drive the motors, get a stable line position, understand PID | 8-10 | 200-400 |
| C Chassis | Have a printed chassis that fits every part | 12-15 | 230-580 |
| D PCB | Have a working Rev A board | 12-15 | 800-2,500 (order, shipping and duties) |
| E Build and tune | Run laps and tune them | 10-15 | 130-150 (tape, paper, foam tape) |
| F Competition | Be ready for race day | 3-5 | 0-100 |

Spend figures are rough estimates for Mumbai and online ordering. Check them when you order.

---

## Part A: Foundations

In exam mode you can do all of Part A in short sessions. It needs a ruler, a multimeter and your parts, nothing else.

### Chapter 1: How a line follower works

**The mental model.** Imagine driving at night with a torch pointing at the road ahead. You see where the lane edge is, decide how far you drifted, and steer a little. A line follower does exactly this, about a thousand times every second:

**SENSE** (where is the line?) then **DECIDE** (how far off am I, and which way?) then **ACT** (run one wheel faster than the other).

| Step | Job | Your part |
| --- | --- | --- |
| Sense | Measure where the line is | RLS-08 (8 sensors) |
| Decide | Turn that into a steering correction | Arduino Nano running your code |
| Act | Drive the two wheels at different speeds | TB6612 plus 2 N20 motors |
| Power | Keep the voltage steady | 2 LiPo cells plus a 5V buck converter |
| Body | Hold everything rigid and light | Your chassis and your PCB |

**Why loop speed matters (worked example).** A robot moving at 1 m/s travels 1 mm every millisecond. If your loop runs at 1,000 times a second, you correct once per millimetre. If it runs at 100 times a second, you correct once per 10 mm, and the robot can drift a long way between corrections. Faster loops give smoother, faster robots.

**Sensor geometry.** Your 8 sensors sit in a row with a gap between them called the **pitch** (call it $p$). The robot does not just ask which sensor sees the line. It takes a weighted average of all 8 readings (Part B), which gives a position far finer than one sensor width. Two things to check on your board:

- **Pitch versus line width.** If the gap between sensors is bigger than the line is wide, the line can slip between two sensors and the robot goes blind. Measure $p$ and compare it with your competition's line width.
- **Sensor height.** The RLS-08 works best about 3 mm above the floor (the listing says the recommended maximum is about 6 mm). Your chassis must hold that height reliably.

**What actually wins competitions.** In most beginner and college events, the order is: finish every run without leaving the line first, then be fast (rule of thumb).

| Factor | Why it matters | Where you handle it |
| --- | --- | --- |
| Reliability | One lost line can end a run | Parts B, D, E |
| Calibration | Lighting changes at every venue | Chapter 8 |
| Grip (tyres) | Slipping wheels make every tuning value wrong | Chapters 10 and 18 |
| Weight and balance | Light, low robots accelerate and turn better | Part C |
| PID tuning | Smooth steering at speed | Chapters 9 and 18 |
| Speed profile | Fast on straights, slow in turns | Chapter 18 |
| Knowing the rules | Line width, intersections and gaps change the code | Part F |

**The physics of fast corners (worked example).** A robot can only take a corner as fast as its tyres hold. The limit is roughly $v = \sqrt{\text{grip} \times g \times \text{radius}}$. With a grip coefficient of 0.8 (an example value, not measured) and a corner radius of 0.20 m: $v = \sqrt{0.8 \times 9.81 \times 0.20} = \sqrt{1.57} \approx 1.25\text{ m/s}$. Two lessons:

1. **Grip matters more than weight for the corner limit.** The robot's mass cancels out of this formula.
2. **Light weight still helps,** because the same motor force speeds up and brakes a light robot faster.

### Chapter 2: Your parts

| Part | Job | Key facts | What to verify yourself |
| --- | --- | --- | --- |
| Arduino Nano | The brain | ATmega328P at 16 MHz, 5V logic, 10-bit analog readings (0 to 1023), 8 analog inputs (A6 and A7 are analog-only), PWM on D3, D5, D6, D9, D10, D11. One analogRead takes about 0.1 ms at default settings (rule of thumb) | Clone bootloader type (Part B) |
| TB6612FNG | Motor driver (two H-bridges) | Motor supply roughly 4.5 to 13.5 V, about 1.2 A continuous and 3.2 A peak per channel, logic 2.7 to 5.5 V, STBY pin enables it | Your breakout's own page, and its pin order |
| N20 motors | The wheels' drive | Small geared DC motors, body about 12 × 10 mm, 3 mm D-shaft is typical. Voltage and RPM vary a lot | Label voltage and RPM, body size, shaft size |
| RLS-08 | The eyes | 8 TCRT5000 infrared pairs, analog and digital outputs per sensor, best at about 3 mm above the floor, 3 mm mounting holes. One seller lists the board as 126 × 26 mm | Real size, hole spacing, pin names, pitch |
| LiPo cell (×2) | Energy | 3.7 V nominal, 4.2 V full. Two in series give about 7.4 V nominal, 8.4 V full | Each cell's size, weight, capacity |
| Buck module | Steps the battery down to 5V | You set the output with a multimeter | Size, input range, current rating |

### Chapter 3: The electronics you need

You can build the whole robot with these seven ideas:
1. **Voltage, current, ground (the water picture):** All parts must share one ground.
2. **Ohm's law:** $I = V / R$.
3. **Voltage divider:** $V_{out} = V_{in} \times R_2 / (R_1 + R_2)$.
4. **PWM:** The Nano switches motor power rapidly (duty cycle determines average voltage).
5. **Decoupling capacitors:** 100µF and 0.1µF across VM and GND to prevent resets.
6. **LiPo cells and series packs:** Measure both cells; never discharge below 3.6V resting.
7. **Power budget and the buck module:** Buck supplies clean 5.0V for Nano and logic.

---

## Part B: Bench bring-up

Prove each part on a breadboard with Nano, TB6612, sensor, and motors before CAD or PCB design.

### Chapter 4: Tools and setup
Install Arduino IDE, install CH340 driver if needed, upload Blink, verify Serial at 115200 baud.

### Chapter 5: Read the sensor
Connect S1–S8 to A0–A7. Run finger test to ensure array orientation matches code indexing.

### Chapter 6: Drive the motors
Connect TB6612 to Nano pins. Measure motor dead-zone (PWM where rotation begins). Cap maximum PWM to 180 (~70%) if motors are 6V and pack is 8.4V.

### Chapter 7: Run from the battery
Set buck converter to exact 5.0V before connecting Nano. Confirm no brownouts when motors spin.

### Chapter 8: Calibration and line position
Sweep sensor across black tape on white paper. Normalize to 0–1000 scale, calculate weighted position:
$$\text{position} = \frac{\sum (n_i \times i \times 1000)}{\sum n_i}$$
Returns 0 to 7000 (centre = 3500). If line is lost ($\sum n_i < 200$), remember last seen edge.

### Chapter 9: Control theory, made simple
Error calculation:
$$\text{error} = \text{position} - 3500$$
Correction:
$$\text{corr} = K_p \times \text{error} + K_i \times \int \text{error} + K_d \times \frac{d(\text{error})}{dt}$$
Run the Python simulation (`simulation/pid_simulation.py`) to observe how $K_d$ eliminates overshoot.

---

## Part C: Chassis from zero

- Model in SolidWorks using parametric equations.
- Layout A: Drive wheels at rear, skid at front, RLS-08 sensor arm extending 60–80 mm forward with height adjustment slots (3–5 mm above floor).
- 3D print test cards for hole tolerances (M2/M3) and N20 motor pockets before printing the complete chassis in PLA.

---

## Part D: Your own PCB

- Design Rev A in KiCad using plug-in headers for Nano and TB6612 module for serviceability.
- Import chassis board outline via DXF to Edge.Cuts.
- Maintain star grounding at battery terminal and wide tracks (0.5–1.0 mm) for motor power.
- Verify 1:1 paper printout with real components before ordering Gerbers.

---

## Part E: Build, firmware and tuning

- Assemble in logical stages (motors -> chassis -> PCB -> battery -> sensor).
- Run the 4-state state machine firmware (`IDLE`, `CALIBRATING`, `READY`, `RUNNING`) with a fixed 1 kHz loop (`1000 µs`).
- Tune $K_p$ first on an oval track, add $K_d$ to suppress oscillations, test curves, and progressively raise speed.

---

## Part F: Competition

- Check event rules: line width, turn radii, timeouts, scoring criteria.
- Race day protocol: measure LiPo cell voltages, recalibrate sensors on the official track, clean tyres with tape, and start with safe, verified tuning parameters.
