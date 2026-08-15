# SmartRoll – Hall Sensor Test Measurement Log

**Test ID:** SR-HALL-TEST-002  
**Status:** OPEN  
**Operator:**  
**Date:**  
**Arduino:** Nano / ATmega328P  
**Firmware:** `arduino/SR-HALL-TEST-002.ino`

---

## 1. Components actually tested

| Item | Part number / description | Qty | Result |
|---|---|---:|---|
| Hall A | A3144 | 1 | OPEN |
| Hall B | A3144 | 1 | OPEN |
| Magnet |  |  | OPEN |
| Magnet dimensions |  |  | OPEN |
| Magnet grade |  |  | OPEN |
| Resistor | 10 kΩ | 2 | OPEN |
| Capacitor | 100 nF | 2 | OPEN |

---

# T1 – Single Hall sensor / single magnet

Purpose: verify basic switching and determine practical switching/release distance.

## Hall A

| Trial | Magnet orientation | Approach distance mm | State | Notes |
|---:|---|---:|---|---|
| 1 |  |  |  |  |
| 2 |  |  |  |  |
| 3 |  |  |  |  |
| 4 |  |  |  |  |
| 5 |  |  |  |  |

**Hall A switching distance:** ______ mm  
**Hall A release distance:** ______ mm  
**Stable working gap selected:** ______ mm

## Hall B

| Trial | Magnet orientation | Approach distance mm | State | Notes |
|---:|---|---:|---|---|
| 1 |  |  |  |  |
| 2 |  |  |  |  |
| 3 |  |  |  |  |
| 4 |  |  |  |  |
| 5 |  |  |  |  |

**Hall B switching distance:** ______ mm  
**Hall B release distance:** ______ mm  
**Stable working gap selected:** ______ mm

---

# T2 – Magnet polarity / orientation

Test the magnet in the orientation intended for radial mounting.

| Test | Magnet orientation | Hall A | Hall B | Comment |
|---:|---|---|---|---|
| 1 | Face / pole 1 |  |  |  |
| 2 | Face / pole 2 |  |  |  |
| 3 | Radial orientation |  |  |  |

**Selected orientation:** ____________________  
**Reason:** _________________________________

---

# T3 – Hall A/B phase test

Initial hypothesis: **45° sensor separation only. This is NOT a confirmed design value.**

| Trial | A/B angle | Air gap mm | Result | Notes |
|---:|---:|---:|---|---|
| 1 | 45° |  |  |  |
| 2 |  |  |  |  |
| 3 |  |  |  |  |
| 4 |  |  |  |  |
| 5 |  |  |  |  |

### Observed sequence

Direction 1:

```text
00 → ____ → ____ → ____ → 00
```

Direction 2:

```text
00 → ____ → ____ → ____ → 00
```

**Is the sequence unambiguous?** YES / NO  
**Selected A/B angle:** ______ °  
**Reason:** _________________________________

---

# T4 – Two magnets, 180° arrangement

| Parameter | Result |
|---|---:|
| Magnet spacing | 180° |
| Hall A/B angle | ______ ° |
| Air gap | ______ mm |
| Transitions / revolution | ______ |
| Expected transitions / revolution | ______ |

## Direction 1

Observed sequence:

```text
```

**Valid?** YES / NO

## Direction 2

Observed sequence:

```text
```

**Valid?** YES / NO

---

# T5 – Repetition / reliability

Perform at least **20 complete revolutions in each direction**.

| Direction | Revolutions | Missed transitions | Extra/false transitions | Invalid transitions | Result |
|---|---:|---:|---:|---:|---|
| Direction 1 | 20 |  |  |  |  |
| Direction 2 | 20 |  |  |  |  |

**Stationary false switching observed:** YES / NO  
**Notes:** __________________________________________

---

# T6 – Speed test

Increase rotation speed until either the expected maximum practical speed is exceeded or the sensor system begins to fail.

| Test | Approx. speed | Transitions OK | Missed | Invalid | Notes |
|---:|---:|---|---:|---:|---|
| 1 |  |  |  |  |  |
| 2 |  |  |  |  |  |
| 3 |  |  |  |  |  |
| 4 |  |  |  |  |  |
| 5 |  |  |  |  |  |

**Maximum tested speed:** ____________________  
**Maximum reliable speed:** __________________

---

# T7 – Final candidate assessment

## Mechanical

- [ ] Magnet fits intended radial pocket
- [ ] Selected air gap is mechanically achievable
- [ ] Tolerances leave sufficient magnetic margin
- [ ] Magnet cannot move in the test/final carrier

## Electrical

- [ ] Hall A switches reliably
- [ ] Hall B switches reliably
- [ ] No significant stationary false switching
- [ ] A/B transitions are unambiguous
- [ ] Both rotation directions are distinguishable
- [ ] No missed transitions at required speed

## Decision

**Candidate status:**

- [ ] APPROVED
- [ ] CONDITIONAL
- [ ] REJECTED

**Approved Hall part:** __________________________  
**Approved magnet part:** ________________________  
**Approved magnet dimensions:** __________________  
**Approved magnet orientation:** __________________  
**Approved air gap:** ____________________________  
**Approved Hall A/B angle:** ______________________  
**Transitions per revolution:** ____________________

**Reason / comments:**

```text

```

---

# Evidence references

Record the filenames of photographs, videos, serial captures and other evidence. Do not overwrite original evidence.

| Evidence ID | Filename | Test stage | Description |
|---|---|---|---|
| E01 |  |  |  |
| E02 |  |  |  |
| E03 |  |  |  |
| E04 |  |  |  |
| E05 |  |  |  |

## Important

Do not change the final SmartRoll mechanical specification based on an unrecorded observation. Any value that becomes a design requirement must be entered here first and then transferred to the appropriate project specification document.