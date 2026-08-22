# SmartRoll – Hall Sensor Test Measurement Log

**Test ID:** SR-HALL-TEST-002  
**Status:** OPEN  
**Arduino:** Nano / ATmega328P  
**Firmware:** two-Hall test firmware for direction tests.

---

## 1. Components actually tested

| Item | Part number / description | Qty | Result |
|---|---|---:|---|
| Hall A | A3144 | 1 | Tested |
| Hall B | A3144 | 1 | Tested |
| Magnet |  | 2 | Tested |
| Magnet arrangement | 180° apart | 2 | Confirmed |
| Magnet polarization | Same orientation | 2 | Confirmed |
| Resistor | 10 kΩ | 2 | Tested |
| Capacitor | 100 nF | 2 | Tested |

---

# T1 – Single Hall sensor / single magnet

Purpose: verify basic switching and determine practical switching/release distance.

**Status:** Prepared for measurement; results to be entered separately.

---

# T2 – Magnet polarity / orientation

The two test magnets are mounted with the same polarization and their magnetic faces directed toward the Hall sensors.

**Selected test arrangement:** same polarization, two magnets 180° apart.

---

# T3 – Hall A/B phase test

The purpose of this test is to determine the Hall A/B angular separation that produces an unambiguous direction-dependent state sequence.

## T3.1 – 90° Hall separation

**Configuration:**

- 2 magnets, 180° apart
- same magnet polarization
- Hall A/B separation: **90°**
- rotation tested: clockwise
- two test runs

**Observed result:**

The recorded sequence repeatedly contains the states `10`, `11` and `01`, while `00` does not occur. The sequence therefore does not form the desired four-state quadrature cycle.

Observed pattern, simplified:

```text
10 → 11 → 01 → 11 → 10 → 11 → 01 → ...
```

**Assessment:** 90° separation is **not accepted as the final geometry** based on this test.

---

## T3.2 – 45° Hall separation

**Configuration:**

- 2 magnets, 180° apart
- same magnet polarization
- Hall A/B separation: **45°**
- 5 complete rotations clockwise
- 5 complete rotations counter-clockwise

**Clockwise result:**

```text
10 → 11 → 01 → 11 → 10 → 11 → 01 → ...
```

**Counter-clockwise result:**

```text
01 → 11 → 10 → 11 → 01 → 11 → 10 → ...
```

The two directions produce opposite state ordering. The test was repeatable over five rotations in each direction and the recorded firmware reported **0 invalid transitions**.

**Assessment:** 45° is a **strong candidate**, but not final.

---

## T3.3 – 30° Hall separation

**Configuration:**

- 2 magnets, 180° apart
- same magnet polarization
- Hall A/B separation: **30°**
- 5 complete rotations clockwise
- 5 complete rotations counter-clockwise

### Clockwise result

The recorded state sequence is:

```text
11 → 10 → 11 → 01 → 11 → 10 → 11 → 01 → ...
```

This pattern repeats consistently through the five clockwise rotations. The recording shows **0 invalid transitions**. fileciteturn29file0L2-L2

### Counter-clockwise result

The recorded state sequence is:

```text
11 → 01 → 11 → 10 → 11 → 01 → 11 → 10 → ...
```

This is the reverse ordering of the clockwise test. The recording again shows **0 invalid transitions**. fileciteturn30file0L2-L2

### Assessment

**30° successfully distinguishes rotation direction.** Both directions produced repeatable opposite state ordering with zero invalid transitions in the recorded tests.

The 30° test therefore passes the basic direction-detection requirement and remains a candidate alongside 45°.

**Important:** The present tests do not yet establish that 30° is mechanically or electrically better than 45°. Air gap, pulse widths, maximum operating speed and tolerance margin still need to be evaluated.

---

# T4 – Two magnets, 180° arrangement

| Parameter | Result |
|---|---:|
| Magnet spacing | 180° |
| Magnet polarization | Same |
| Hall A/B angle – test 1 | 90° |
| Hall A/B angle – test 2 | 45° |
| Hall A/B angle – test 3 | 30° |
| Air gap | Not yet recorded |
| Transitions / revolution | Not yet formally recorded |
| Expected magnet events / revolution | 2 |

### Comparison

| Hall A/B angle | Direction test | Result | Decision |
|---:|---|---|---|
| 90° | Clockwise, 2 runs | No `00`; three-state pattern | Reject as final geometry |
| 45° | CW 5× + CCW 5× | Opposite repeatable ordering | Candidate |
| 30° | CW 5× + CCW 5× | Opposite repeatable ordering | Candidate |

---

# T5 – Repetition / reliability

| Configuration | Direction | Revolutions | Invalid transitions | Result |
|---|---|---:|---:|---|
| 90° | Clockwise | 2 runs | 0 recorded | Three-state pattern |
| 45° | Clockwise | 5 | 0 recorded | PASS / candidate |
| 45° | Counter-clockwise | 5 | 0 recorded | PASS / candidate |
| 30° | Clockwise | 5 | 0 recorded | PASS / candidate |
| 30° | Counter-clockwise | 5 | 0 recorded | PASS / candidate |

The 30° and 45° arrangements both currently satisfy the basic direction-detection test.

**Stationary false switching:** not yet formally tested in the final two-Hall arrangement.

**Important:** These tests prove repeatability at the tested manual rotation speed. They do not yet prove reliable operation at the maximum speed of the real roller blind.

---

# T6 – Speed test

Not yet performed.

The next phase should compare the best remaining geometries at increasing rotation speed and determine whether any transitions are missed.

---

# T7 – Current candidate assessment

## Mechanical

- [ ] Magnet fits intended radial pocket
- [ ] Selected air gap is mechanically achievable
- [ ] Tolerances leave sufficient magnetic margin
- [ ] Magnet cannot move in the test/final carrier

## Electrical

- [x] Hall A switches reliably in the current test
- [x] Hall B switches reliably in the current test
- [ ] No significant stationary false switching formally verified
- [x] A/B transitions are direction-dependent at 45°
- [x] A/B transitions are direction-dependent at 30°
- [x] Both rotation directions are distinguishable at 45°
- [x] Both rotation directions are distinguishable at 30°
- [ ] No missed transitions at required real-world speed

## Current decision

**Candidate status:** CONDITIONAL

**Current Hall candidate:** A3144  
**Current magnet arrangement:** 2 magnets, 180° apart, same polarization  
**Current Hall A/B angle candidates:** **30° and 45°**  
**Air gap:** to be measured/documented  
**Transitions per revolution:** to be formally determined from the final firmware/test method

**Reason:** Both 30° and 45° have demonstrated repeatable and opposite state ordering for clockwise and counter-clockwise rotation. The 90° arrangement is rejected. The next decision should be based on speed, air gap and tolerance testing rather than another arbitrary angle change.

---

# Evidence references

Original evidence is stored in `tests/hall_sensor_test/evidence/` and must not be overwritten.

| Evidence | Test stage | Description |
|---|---|---|
| 90° photographs | T3.1 | Initial 90° Hall A/B arrangement |
| 90° clockwise TXT – 2 runs | T3.1 | Two clockwise tests at 90° |
| 45° photographs | T3.2 | Hall A/B arrangement at 45° |
| `45 stupnu 5krat po smeru hodin.txt` | T3.2 | 5 clockwise rotations at 45° |
| `45 stupnu 5krat proti smeru hodin.txt` | T3.2 | 5 counter-clockwise rotations at 45° |
| 30° photographs | T3.3 | Hall A/B arrangement at 30° |
| `30 stupnu 5krat po smeru hodin.txt` | T3.3 | 5 clockwise rotations at 30° |
| `30 stupnu 5krat proti smeru hodin.txt` | T3.3 | 5 counter-clockwise rotations at 30° |

## Next planned phase

Compare **30° vs. 45°** under controlled air-gap and speed conditions. Do not freeze the final geometry yet.