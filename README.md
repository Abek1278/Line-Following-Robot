# Line Follower Robot

An 8-sensor line follower with PID control, a custom 3D-printed chassis and a custom PCB. It runs on an Arduino Nano.

## How it works

1. **Sense:** the RLS-08 sensor array looks at the floor. Black reflects less light than white.
2. **Decide:** the Nano turns the 8 readings into one position number (0 to 7000, centre = 3500). Then it calculates the error:
   `error = position - 3500`
3. **Act:** PID turns the error into a steering correction. The TB6612 driver speeds up one motor and slows the other:
   `left = speed + correction` and `right = speed - correction`

## Components and specs

| No. | Component | Qty | Spec / setting |
|---|---|---|---|
| 1 | N20 gear motors with wheels | 2 | Read voltage and RPM from the label. Measure shaft and wheel diameter |
| 2 | Rubber grip tyres | 1 set | Measure the diameter with the tyres fitted |
| 3 | N20 motor brackets | 2 | The chassis CAD is drawn around their hole pattern |
| 4 | RLS-08 8-sensor array | 1 | 8 TCRT5000 IR pairs. Use the analog outputs (A0 to A7). Keep it 3 to 5 mm above the floor |
| 5 | Arduino Nano | 1 | ATmega328P, 16 MHz, 5 V logic, 10-bit analog reads (0 to 1023) |
| 6 | TB6612FNG motor driver | 1 | Dual H-bridge. STBY must be HIGH. About 1.2 A continuous per channel (check your breakout) |
| 7 | Single LiPo cells | 2 | 3.7 V each (4.2 V full). In series: 7.4 V (8.4 V full). Measure both, they should match within 0.05 V |
| 8 | LiPo charger | 1 | Charge each cell separately to 4.2 V |
| 9 | Multimeter | 1 | Sets the buck to 5.0 V, checks cells and continuity |
| 10 | LM2596 buck module (adjustable, no display) | 1 | Set the output to exactly 5.0 V before connecting the Nano |
| 11 | Slide switch | 1 | At least 1 A. Switches battery positive |
| 12 | Tactile push buttons | 2 | Start on D2, Calibrate on D10, each to GND |
| 13 | Capacitors | 100 µF 16 V x2, 0.1 µF x6 | 100 µF + 0.1 µF across VM and GND next to the driver. 0.1 µF also on the 5 V rail |
| 14 | LED and 330 Ω resistor | 2 each | Status LED on D12. One is a spare |
| 15 | LiPo battery holder | 1 | Holds both cells in series, two output wires |
| 16 | Battery connector pair (XT30 or JST-XH) | 1 pair | One half on the holder's wires, the other on the breadboard lead and later the PCB |
| 17 | Ball caster | 1 | Its height decides the chassis geometry, so measure it before the CAD |

Tools: soldering iron and flux, wire cutter and stripper, screwdriver set, breadboard, jumper wires (male-male and male-female), USB cable.

## Safety

- Charge each LiPo cell separately and never leave it unattended.
- Measure both cells before joining them.
- Switch the robot off before plugging in USB.
