# Custom PCB Design (Part D - KiCad Rev A)

This directory stores the KiCad project files, schematics, footprints, and production Gerber files for the robot's custom Rev A mainboard.

## Design Philosophy: Modular Rev A
To avoid bricking an entire board on early revisions, Rev A uses female headers to mount off-the-shelf modules:
- **Arduino Nano** (2 × 15-pin female header sockets)
- **TB6612FNG Breakout Board** (female header sockets)
- **5V Step-Down Buck Module** (solder pads or header pins)

## Board Layout Rules
1. **Ground Plane:** Full copper ground pour on the bottom layer.
2. **Star Grounding:** Single ground reference originating at the battery input connector to prevent high motor return currents from creating ground bounce on analog sensor signals.
3. **Trace Widths:**
   - Motor Power & Battery Rails: `0.8 mm – 1.0 mm`
   - Low-current 5V Logic: `0.4 mm`
   - Sensor & Control Signals: `0.25 mm`
4. **Decoupling Capacitors:** Place a `100 µF` electrolytic capacitor and a `0.1 µF` ceramic capacitor directly adjacent to the TB6612 `VM` and `GND` pins. Place a `0.1 µF` capacitor next to the Nano `5V` pin.
5. **Chassis Fitment:** Export chassis outline and mounting hole locations from SolidWorks as a DXF, and import into KiCad on the `Edge.Cuts` layer.

## Pre-Order Verification Checklist
- [ ] Electrical Rules Check (ERC) reports 0 errors.
- [ ] Design Rules Check (DRC) reports 0 errors.
- [ ] 1:1 scale paper print test: Physical Nano, TB6612, buck module, and connectors placed on top to confirm pin pitch and footprints.
- [ ] Battery polarity silkscreen clearly marked on positive and negative terminals.
- [ ] Gerber and drill files zipped and verified inside KiCad Gerber Viewer / online preview.
