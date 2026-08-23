# SmartRoll – Technical Specification

**Project:** SmartRoll  
**Repository:** `lklimt/SmartRoll`  
**Status:** Work in Progress  
**Current mechanical revision:** SR-MEC-001 Rev.F

## 1. Purpose

SmartRoll is an automated control system project for **external roller blinds equipped with ERTE ET 45E tubular motors**.

The project includes the mechanical carrier, magnetic sensing, electronics, firmware, Home Assistant integration, testing and manufacturing documentation.

This document is the technical source of truth for the project.

## 2. Specification Rules

- **CONFIRMED** – directly measured or explicitly confirmed.
- **DECIDED** – explicitly approved design decision.
- **ASSUMED** – temporary assumption; not a final manufacturing dimension.
- **OPEN** – information still requiring measurement or decision.
- **SUPERSEDED** – previous value/design replaced by a newer result.

Unknown dimensions shall be marked `OPEN`, never invented.

FreeCAD macros are implementation tools, not the source of design requirements. A macro must derive fixed dimensions and geometry from this specification and must not silently introduce final values for `OPEN` parameters.

Every substantial mechanical change receives a new revision. Historical revisions remain historical.

# 3. Mechanical System

## 3.1 Roller-blind application

- Application: **external roller blinds**.
- Motor: **ERTE ET 45E tubular motor**.

**Status: CONFIRMED**

## 3.2 Roller-blind shaft

The roller blind uses an octagonal shaft.

| Parameter | Value | Status |
|---|---:|---|
| Shaft type | Octagonal | CONFIRMED |
| Across flats | 16.70 mm | CONFIRMED |
| Surface containing slat feature | 5 + 6.7 + 5 mm | CONFIRMED |
| Carrier clamping zone width | 6 mm | DECIDED |

The 16.70 mm value is the relevant distance across opposing flats. The face containing the slat feature is the primary mechanical reference.

The exact global CAD coordinate convention remains **OPEN** and shall be fixed before the production macro is finalized.

## 3.3 Available space

| Condition | Available space | Status |
|---|---:|---|
| Blind unrolled | approx. 35 mm from wall | CONFIRMED |
| Blind wound | approx. 30 mm effective space | CONFIRMED |

The approximately 30 mm wound condition is the limiting clearance condition unless a later measurement establishes a smaller value.

## 3.4 Stationary and rotating areas

The right-hand side contains a stationary black mounting part including the motor programming button. The rotating part begins approximately **24 mm from the right-hand side**.

The magnetic carrier must not:

- contact the stationary mounting part,
- obstruct the programming button,
- prevent operation or programming of the ERTE ET 45E motor,
- enter the stationary portion of the mechanism.

**Status: CONFIRMED / APPROXIMATE where noted; DECIDED as design constraint.**

# 4. Magnetic Carrier – SR-MEC-001

The carrier is a **two-piece component** clamped around the octagonal shaft. The two halves are installed independently and mechanically joined.

The carrier consists of two functional zones:

1. **Shaft clamping zone** – positively locked to the octagonal shaft.
2. **Magnetic rotating zone** – carries the magnets and provides circular clearance around the stationary mechanism.

The two zones must not be confused: the shaft-clamping geometry follows the octagonal shaft, while the magnetic section uses circular rotating clearance.

**Status: DECIDED**

## 4.1 Shaft clamping

- Octagonal internal geometry matching the shaft.
- Shaft AF: **16.70 mm**.
- Clamping section width: **6 mm**.
- Rotation shall be transmitted positively and shall not rely on friction alone.
- Slat clearance shall be provided on the correct shaft face.

## 4.2 Slat clearance

The slat clearance is rotated **90° relative to the carrier split / M3 joining arrangement**.

The clearance shall be large enough for the actual shaft feature while removing as little carrier material as practical.

Exact depth and detailed shape remain **OPEN** until measured geometry is entered.

## 4.3 M3 clamping

The two carrier halves use **two M3 connections**.

Requirements:

- closed screw bosses,
- accessible M3 screw heads,
- M3 nut pockets with anti-rotation retention,
- hardware outside the rotating clearance,
- sufficient material around the bosses.

The two halves shall clamp the shaft securely without obstructing the magnetic rotation.

# 5. Approved Magnet Arrangement

| Parameter | Value | Status |
|---|---:|---|
| Number of magnets | 2 | DECIDED |
| Angular separation | 180° | DECIDED |
| Magnet orientation | Radial | DECIDED |
| Magnet polarization | Same in tested arrangement | CONFIRMED |
| Nominal magnet size | 4 × 2 mm | DECIDED |

The magnets are positioned opposite each other and inserted radially from the outside of the carrier into secure pockets.

The final pocket depth and exact retention method remain **OPEN** until production verification.

# 6. Approved Hall Sensor Arrangement

The approved Hall arrangement is:

- **2 Hall sensors**, Hall A and Hall B,
- Hall A and Hall B mounted **directly beside each other**,
- no intentional large angular separation between Hall A and Hall B,
- nominal tested air gap: **5 mm**.

This close A/B arrangement is essential to the verified quadrature signal and avoids an unwanted magnetic dead zone between the sensors.

A **10 mm** air gap was experimentally found unreliable. **5 mm** was reliable, including rapid manual rotation faster than the expected roller-blind motor speed.

**Status: APPROVED TEST CONFIGURATION.**

The exact production Hall mounting dimensions and final air gap remain subject to mechanical verification.

# 7. Approved Hall Quadrature Signal

With Hall A and Hall B directly beside each other, the experimentally verified state sequences are:

### Clockwise rotation – CW

```text
11 → 10 → 00 → 01 → 11
```

### Counter-clockwise rotation – CCW

```text
11 → 01 → 00 → 10 → 11
```

All four states occur:

```text
00
01
10
11
```

The two physical directions therefore produce opposite state ordering and can be distinguished unambiguously.

# 8. Approved Direction Decoder

SmartRoll shall use a **2-bit quadrature state-transition decoder** based on the complete previous-state → new-state transition.

## 8.1 CW transitions

```text
11 → 10
10 → 00
00 → 01
01 → 11
```

Each is one valid CW movement increment.

## 8.2 CCW transitions

```text
11 → 01
01 → 00
00 → 10
10 → 11
```

Each is one valid CCW movement increment.

## 8.3 Invalid transitions

Any transition not listed above is **INVALID**.

An invalid transition shall:

- increment a diagnostic invalid-transition counter,
- not count as normal movement,
- not by itself reverse the detected direction.

The decoder must not determine direction from either Hall signal individually.

## 8.4 Physical direction vs blind movement

`CW` and `CCW` describe physical rotation only.

The final system shall map physical direction to:

- **WIND** – movement toward the rolled-up position.
- **UNWIND** – movement toward the extended position.

This mapping shall be configurable during installation/calibration and shall not be hard-coded from assumed sensor orientation.

# 9. Restart and Power-Failure Behavior

The controller must reacquire direction after restart or power failure without requiring the previous direction to survive in RAM.

Startup procedure:

1. Read the current Hall A/B state.
2. Store it as `previousState`.
3. Set direction to `UNKNOWN`.
4. Wait for the next Hall state change.
5. Decode the valid transition.
6. Set direction to CW or CCW.
7. Continue relative movement counting.

Example:

```text
Restart
  ↓
Read A/B = 11
  ↓
Direction = UNKNOWN
  ↓
11 → 10
  ↓
Direction = CW
```

The opposite valid transition similarly establishes CCW.

The controller shall not infer direction from the motor command alone and shall not assume that the blind is stationary at startup.

**Status: DECIDED and experimentally verified on Arduino Nano.**

# 10. Absolute Position After Power Failure

Exact absolute blind position recovery immediately after a power failure is **not required**.

The ERTE ET 45E motor has programmed upper and lower travel limits. Hall sensing is therefore used for relative rotation and movement counting.

A power failure during movement may temporarily leave the software position offset from the physical position. This is acceptable.

After complete travel to a known upper or lower motor endpoint, SmartRoll shall resynchronize its software position to that known endpoint.

No additional absolute-position sensor is required for this recovery concept.

**Status: DECIDED.**

# 11. Relative Position Counting – OPEN

Valid quadrature transitions provide signed relative movement counts.

The system shall distinguish between:

- raw Hall state changes,
- valid quadrature transitions,
- signed shaft rotation count,
- calibrated blind position.

The following values still require measurement and shall not be guessed:

- valid transitions per shaft revolution,
- shaft revolutions per full blind travel,
- conversion between Hall count and blind position.

# 12. Verification Completed

The approved Hall concept has been experimentally verified on the Merkur-based development fixture using an Arduino Nano.

Completed tests:

1. Two magnets 180° apart.
2. Hall A/B sensors directly beside each other.
3. CW and CCW state-sequence verification.
4. Five rotations CW and five rotations CCW.
5. Direction changes during operation.
6. Rapid manual rotation faster than expected roller-blind motor speed.
7. 5 mm air-gap reliability test – reliable.
8. 10 mm air-gap test – unreliable.
9. Reset while operating CW.
10. Reset while operating CCW.
11. Correct direction reacquisition after reset.
12. No invalid transitions in the recorded normal-operation tests.

The quadrature direction algorithm is therefore **experimentally validated on the Arduino Nano test platform**.

Evidence is stored in:

```text
/tests/hall_sensor_test/evidence/
```

Detailed measurement history is stored in:

```text
/tests/hall_sensor_test/measurement_log.md
```

# 13. Test Platform vs Production Platform

The Arduino Nano / ATmega328P is **test hardware only**.

The final SmartRoll control unit will use an **ESP microcontroller**.

Therefore:

- Arduino pin assignments are not production assignments.
- Arduino-specific timing/interrupt implementation is not a production requirement.
- The validated quadrature transition behavior is the functional requirement.
- Final ESP firmware shall implement equivalent behavior using ESP-compatible GPIO handling.

# 14. Production Electrical Parameters – OPEN

The following require final component selection and verification:

- Hall sensor part number,
- final ESP model,
- ESP GPIO assignments,
- Hall supply voltage,
- Hall output type/polarity,
- pull-up arrangement,
- input protection,
- filtering/debounce if required,
- maximum expected transition frequency.

# 15. Mechanical Verification Requirements

Before a carrier revision is considered production-ready, verify:

1. correct 16.70 mm shaft engagement,
2. correct slat clearance,
3. correct orientation of the carrier halves,
4. secure M3 clamping,
5. accessible M3 screw heads,
6. correct nut retention,
7. secure magnet retention,
8. magnets remain 180° apart,
9. Hall A/B remain directly beside each other,
10. production air gap is within the validated magnetic operating range,
11. no intrusion into circular rotating clearance,
12. no interference with the stationary mounting part,
13. programming button remains accessible,
14. sufficient clearance in the approximately 30 mm wound condition,
15. secure torque transmission from shaft to carrier.

A CAD model alone does not constitute mechanical verification.

# 16. Parameters That Must NOT Be Guessed

The following shall remain `OPEN` unless explicitly measured or decided:

- exact stationary-holder diameter,
- exact distance between stationary holder and wall,
- exact programming-button geometry,
- exact slat-feature geometry,
- exact depth and width of slat clearance,
- final carrier outer diameter,
- final magnetic-section dimensions,
- final magnet-pocket geometry,
- final M3 boss dimensions,
- final printing tolerances,
- final material and print orientation,
- final Hall mounting dimensions,
- final production air gap,
- position calibration constants.

# 17. FreeCAD Macro Requirements

Production macros shall:

- contain clearly named parameters,
- separate confirmed/decided dimensions from `OPEN` dimensions,
- generate both carrier halves,
- preserve the 16.70 mm octagonal shaft geometry,
- preserve the 6 mm clamping-zone width,
- include the slat clearance in the correct orientation,
- provide circular rotating clearance in the magnetic section,
- place two radial magnet pockets 180° apart,
- preserve the approved Hall A/B installation geometry,
- provide two functional M3 joining bosses,
- provide complete M3 screw-head seats,
- provide anti-rotation nut pockets,
- keep M3 hardware outside the rotating clearance,
- keep the programming button accessible,
- expose `OPEN` dimensions at the top of the macro,
- never silently replace confirmed dimensions with approximations.

# 18. Superseded Hall Experiments

Previous experiments using Hall sensors separated by approximately **30°, 45° or 90°** are now **SUPERSEDED**.

They are retained only as historical evidence. They shall **not** be used as:

- production mechanical references,
- firmware references,
- direction-algorithm references,
- geometry references for the final Hall installation.

In particular, the former 30° arrangement and its associated state machine are obsolete.

The approved reference is exclusively:

```text
2 magnets, 180° apart
Hall A + Hall B directly beside each other
5 mm successful test air gap

CW : 11 → 10 → 00 → 01 → 11
CCW: 11 → 01 → 00 → 10 → 11
```

# 19. Current Approved Hall Status

### APPROVED

- external roller blinds with ERTE ET 45E motors,
- 2 magnets,
- magnets 180° apart,
- same magnet polarization in tested arrangement,
- radial magnet orientation,
- 2 Hall sensors,
- Hall A/B directly beside each other,
- successful 5 mm test air gap,
- 2-bit quadrature decoding,
- complete-state transition direction detection,
- invalid-transition handling,
- restart direction reacquisition,
- relative movement counting concept,
- no requirement for exact absolute position immediately after power failure,
- endpoint resynchronization using motor limits,
- Arduino Nano as test platform,
- ESP as final controller platform.

### OPEN

- final Hall sensor selection,
- final ESP model and GPIO assignment,
- production Hall mounting dimensions,
- production air gap,
- electrical interface details,
- transitions per revolution calibration,
- blind-position calibration,
- final mechanical carrier verification.
