# CAD & Mechanical Design (Part C)

This folder contains SolidWorks CAD models, 3D printing configurations, and exported STL files.

## Design Architecture (Layout A)
- **Drive Configuration:** 2 × N20 micro metal gear motors located at the rear axle.
- **Support:** Low-friction front skid mounted beneath the front of the base plate.
- **Sensor Extension:** Forward arm holding the RLS-08 sensor bar **60 to 80 mm** in front of the drive axle.
- **Adjustment:** Sensor mounting bracket features vertical slots allowing height tuning between **3 mm and 5 mm** above the track.

## Required CAD Parts Checklist
1. `Motor_Mount.SLDPRT` (Single clamp part, printed twice)
2. `Base_Plate.SLDPRT` (Houses battery holder, standoffs for PCB, skid mount)
3. `Sensor_Arm_Mount.SLDPRT` (Slotted for height and forward extension)
4. `Battery_Pocket.SLDPRT` (Dual 1S LiPo pocket with strap guides)
5. `Line_Follower_Assembly.SLDASM` (Mated assembly with interference checking)

## 3D Printing Recommendations
- **Material:** PLA (stiff, lightweight, minimal warping)
- **Perimeters:** 3 solid walls
- **Infill:** 20% to 30% (Gyroid or Grid)
- **Layer Height:** 0.20 mm
- **Hole Tolerances:**
  - M2 screw clearance: 2.2 – 2.4 mm
  - M3 screw clearance: 3.2 – 3.4 mm
- **Pre-print Tests:** Run test cards for hole tolerances and N20 motor pocket clearances before printing full chassis parts.
