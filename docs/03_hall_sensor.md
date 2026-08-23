# SmartRoll – Hall Sensor and Direction Detection

**Status: APPROVED FUNCTIONAL SPECIFICATION**

## 1. Purpose

The Hall system detects rotation of the roller-blind shaft, determines its direction and provides the basis for relative position counting.

SmartRoll controls **external roller blinds with ERTE ET 45E tubular motors**.

## 2. Approved Mechanical Hall Arrangement

The rotating magnetic carrier contains:

- **2 magnets**,
- magnets positioned **180° apart**,
- magnets with the same polarization in the tested arrangement,
- radial magnet orientation,
- **2 Hall sensors: Hall A and Hall B**,
- Hall A and Hall B mounted **directly beside each other** so that there is no intentional angular dead zone between the sensors.

The Hall sensors observe the rotating magnetic field of the two magnets.

### Approved test geometry

| Parameter | Value | Status |
|---|---:|---|
| Magnets | 2 | **APPROVED** |
| Magnet separation | 180° | **APPROVED** |
| Magnet polarization | Same in tested arrangement | **APPROVED** |
| Hall sensors | 2 | **APPROVED** |
| Hall arrangement | A/B directly beside each other | **APPROVED** |
| Test air gap | 5 mm | **APPROVED TEST VALUE** |

The 5 mm air gap is the successful tested value. A 10 mm gap was found unreliable on the test fixture.

The exact production mounting dimensions remain subject to the final mechanical carrier design.

## 3. Experimentally Verified Hall Sequences

With the Hall sensors mounted directly beside each other, the following sequences were measured repeatedly.

### Clockwise

```text
11 → 10 → 00 → 01 → 11
```

### Counter-clockwise

```text
11 → 01 → 00 → 10 → 11
```

The two directions therefore produce opposite 2-bit quadrature sequences.

All four Hall states are present:

```text
00
01
10
11
```

Repeated tests produced no invalid transitions under normal manual operation.

## 4. Approved Direction Decoder

SmartRoll shall use a **2-bit quadrature state-transition decoder**.

### Clockwise transitions

| Previous | New | Direction |
|---|---|---|
| `11` | `10` | CW |
| `10` | `00` | CW |
| `00` | `01` | CW |
| `01` | `11` | CW |

### Counter-clockwise transitions

| Previous | New | Direction |
|---|---|---|
| `11` | `01` | CCW |
| `01` | `00` | CCW |
| `00` | `10` | CCW |
| `10` | `11` | CCW |

Any transition not listed above is **invalid**.

An invalid transition:

- increments the diagnostic invalid-transition counter,
- does not count as a normal movement transition,
- must not by itself reverse the detected direction.

The decoder must evaluate the complete previous-state → new-state transition, not the state of either Hall sensor individually.

## 5. Direction Meaning

`CW` and `CCW` are physical directions of rotation only.

The production firmware shall map the physical direction to the logical SmartRoll functions:

- **WIND** – blind moves toward the rolled-up position,
- **UNWIND** – blind moves toward the extended position.

This mapping must be configurable during installation/calibration and must not be assumed from the sensor wiring.

## 6. Startup and Power-Failure Recovery

The controller must determine direction correctly after a restart, including a power failure.

The previous direction must **not** be required to survive in RAM.

### Required startup sequence

1. Read current Hall A/B state.
2. Store the state as `previousState`.
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

Opposite direction:

```text
Restart
  ↓
Read A/B = 11
  ↓
Direction = UNKNOWN
  ↓
11 → 01
  ↓
Direction = CCW
```

The firmware must not infer direction from the motor command alone and must not assume that the blind is stationary at startup.

## 7. Absolute Position After Power Failure

SmartRoll does **not** require exact absolute blind position recovery immediately after a controller power failure.

The ERTE ET 45E motor has programmed upper and lower travel limits. Hall sensing therefore provides relative rotation and movement information.

A power failure during movement may leave the software position temporarily offset from the physical position. This is acceptable.

After a complete movement to a known upper or lower motor endpoint, SmartRoll shall resynchronize its software position to that known endpoint.

No additional absolute-position sensor is required for this purpose.

## 8. Position Counting

Each valid quadrature transition may be used for relative position counting.

The firmware shall distinguish between:

- raw Hall state changes,
- valid quadrature transitions,
- signed relative rotation count,
- calibrated roller-blind position.

The conversion from quadrature transitions to shaft revolutions and blind position must be measured experimentally and shall not be guessed.

**Status: OPEN – calibration value.**

## 9. Verification Completed

The following tests have been completed successfully on the Merkur-based development fixture:

1. Hall detection with two magnets 180° apart.
2. Hall A/B sensors positioned directly beside each other.
3. Basic CW and CCW state-sequence verification.
4. Five rotations CW and five rotations CCW.
5. Direction changes during operation.
6. Rapid manual rotation faster than the expected roller-blind motor speed.
7. Air-gap test: 5 mm reliable; 10 mm unreliable.
8. Controller reset while operating CW.
9. Controller reset while operating CCW.
10. Correct direction reacquisition after reset.
11. No invalid transitions during the recorded normal-operation tests.

The quadrature direction algorithm is therefore **experimentally validated on the Arduino Nano test platform**.

Evidence is stored in:

```text
/tests/hall_sensor_test/evidence/
```

The measurement history is stored in:

```text
/tests/hall_sensor_test/measurement_log.md
```

## 10. Test Platform vs Production Platform

The current experiments use an **Arduino Nano / ATmega328P** only as a development and measurement platform.

The final SmartRoll controller will use an **ESP microcontroller**.

Therefore:

- Arduino firmware is test firmware only,
- Arduino pin numbers are not production assignments,
- Arduino-specific interrupt implementation is not a production requirement,
- the validated quadrature state-transition logic is the functional requirement,
- the final ESP firmware shall implement equivalent behavior using ESP-compatible GPIO handling.

## 11. Production Electrical Requirements

The final production design shall document:

- exact Hall sensor part number,
- supply voltage,
- output type and polarity,
- pull-up arrangement,
- ESP GPIO assignment,
- input protection,
- electrical filtering/debounce if required,
- maximum expected transition frequency.

These items remain **OPEN** until the production Hall sensor and ESP hardware are selected.

## 12. Important Design Rule

The approved direction-detection geometry is **Hall A and Hall B directly beside each other**.

Previous experiments with Hall sensors separated by approximately 30°, 45° or 90° are **superseded** and shall not be used as design references for the production system.

The approved reference is the configuration that produced the experimentally verified quadrature sequences:

```text
CW :  11 → 10 → 00 → 01 → 11
CCW:  11 → 01 → 00 → 10 → 11
```

## 13. Current Approved Status

### APPROVED

- 2 Hall sensors.
- Hall A/B directly beside each other.
- 2 magnets.
- Magnets 180° apart.
- Same magnet polarization in the tested arrangement.
- Radial magnet orientation.
- 5 mm successful test air gap.
- 2-bit quadrature decoding.
- Direction from complete A/B state transitions.
- Invalid transitions excluded from normal counting.
- Direction starts as `UNKNOWN` after restart.
- Direction is reacquired from subsequent valid Hall transition.
- Exact absolute position after power failure is not required.
- Motor upper/lower limits are used for later position resynchronization.
- Arduino Nano is test hardware only.
- Final controller is ESP-based.

### OPEN

- final production Hall sensor,
- final ESP model,
- final ESP GPIO assignment,
- final production sensor mounting dimensions,
- final production air gap,
- electrical filtering/debounce implementation,
- transitions per shaft revolution,
- position calibration,
- WIND/UNWIND configuration storage.
