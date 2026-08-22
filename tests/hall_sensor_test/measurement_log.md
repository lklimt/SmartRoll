# SmartRoll – Hall Sensor Test Measurement Log

**Test ID:** SR-HALL-TEST-002  
**Status:** OPEN  
**Operator:**  
**Arduino:** Nano / ATmega328P  
**Firmware:** `arduino/SR-HALL-TEST-003_SINGLE_HALL.ino` for single-Hall tests; two-Hall firmware for direction tests.

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

**Evidence:** see `evidence/` – clockwise 90° test recording and photographs of the 90° arrangement.

---

## T3.2 – 45° Hall separation

**Configuration:**

- 2 magnets, 180° apart
- same magnet polarization
- Hall A/B separation: **45°**
- 5 complete rotations clockwise
- 5 complete rotations counter-clockwise

**Clockwise result:**

Repeated sequence:

```text
10 → 11 → 01 → 11 → 10 → 11 → 01 → ...
```

**Counter-clockwise result:**

Repeated sequence:

```text
01 → 11 → 10 → 11 → 01 → 11 → 10 → ...
```

The two directions produce opposite state ordering. The test was repeatable over five rotations in each direction and the recorded firmware reported **0 invalid transitions**.

**Assessment:** 45° is a **strong candidate for the Hall A/B geometry**, but is not yet declared final. Further tests at other angles and speeds are required.

**Evidence:**

- `evidence/45 stupnu 5krat po smeru hodin.txt`
- `evidence/45 stupnu 5krat proti smeru hodin.txt`
- corresponding 45° setup photographs in `evidence/`

---

# T4 – Two magnets, 180° arrangement

| Parameter | Result |
|---|---:|
| Magnet spacing | 180° |
| Magnet polarization | Same |
| Hall A/B angle – test 1 | 90° |
| Hall A/B angle – test 2 | 45° |
| Air gap | Not yet recorded |
| Transitions / revolution | Not yet formally recorded |
| Expected magnet events / revolution | 2 |

### Comparison

| Hall A/B angle | Direction test | Result | Decision |
|---:|---|---|---|
| 90° | Clockwise, 2 runs | No `00`; three-state pattern | Reject as final geometry |
| 45° | CW 5× + CCW 5× | Opposite repeatable ordering | Candidate |

---

# T5 – Repetition / reliability

The 45° configuration has already been tested for **5 complete rotations in each direction**.

| Configuration | Direction | Revolutions | Missed transitions | Invalid transitions | Result |
|---|---|---:|---:|---:|---|
| 90° | Clockwise | 2 runs | 0 recorded | 0 recorded | Three-state pattern |
| 45° | Clockwise | 5 | 0 recorded | 0 recorded | PASS / candidate |
| 45° | Counter-clockwise | 5 | 0 recorded | 0 recorded | PASS / candidate |

**Stationary false switching:** not yet formally tested in the final two-Hall arrangement.

**Important:** The 45° result proves repeatability at the tested manual rotation speed. It does not yet prove reliable operation at the maximum speed of the real roller blind.

---

# T6 – Speed test

Not yet performed.

The speed test must be performed after the best Hall A/B geometry has been selected.

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
- [x] Both rotation directions are distinguishable at 45°
- [ ] No missed transitions at required real-world speed

## Current decision

**Candidate status:** CONDITIONAL

**Current Hall candidate:** A3144  
**Current magnet arrangement:** 2 magnets, 180° apart, same polarization  
**Current Hall A/B angle candidate:** 45°  
**Air gap:** to be measured/documented  
**Transitions per revolution:** to be formally determined from the final firmware/test method

**Reason:** 45° has demonstrated repeatable and opposite state ordering for clockwise and counter-clockwise rotation, but additional angle and speed tests are required before freezing the mechanical design.

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

## Next planned test

**30° Hall A/B separation**, keeping all other conditions unchanged. Perform 5 complete rotations clockwise and 5 counter-clockwise and record the complete serial output in `evidence/`.