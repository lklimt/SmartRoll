# SmartRoll – Complete Test Sequence

This document is the master validation sequence for the SmartRoll project. Tests are performed incrementally so that each subsystem is proven independently before subsystems are combined.

## Principle

**Do not move to the next integration stage until the preceding stage has passed.**

The purpose is to isolate faults and avoid changing several variables at once.

---

## Test 1 – Hall sensor hardware and geometry

**Location:** `tests/hall_sensor_test/`

### Objective

Verify the selected Hall sensors and the two-magnet mechanical arrangement.

### Confirmed configuration

- 2 magnets, 180° apart on the rotating disc
- 2 Hall sensors placed directly beside each other to avoid a dead zone
- approximately 5 mm air gap
- both magnets with the same magnetic polarity toward the Hall sensors

### Validation performed

The earlier experiments with different angles and air gaps established the practical geometry. The 10 mm air gap proved unreliable, while approximately 5 mm was reliable, including fast manual rotation.

The earlier exploratory tests with 90°/45°/30° arrangements are historical evidence only and are not part of the final approved algorithm.

### Status

**PASS – Hall hardware and final mechanical arrangement accepted.**

---

## Test 2 – ESP32 + Hall A/B quadrature

**Location:** `tests/esp32_hall_test/`

### Objective

Port the approved quadrature decoder from the Arduino test platform to ESP32.

### Test sequence

1. ESP32 restart.
2. Establish initial Hall state.
3. Rotate 5 complete turns in one direction.
4. Short pause.
5. Rotate 5 complete turns back.
6. Stop.

### Acceptance

- correct CW/CCW recognition
- no false direction changes
- no invalid transitions
- 40 transitions per 5 turns in each direction for the tested geometry
- relative position returns to zero after equal movement in opposite directions
- decoder remains correct after restart

### Result

**PASS – ESP32 Hall subsystem accepted.**

Detailed results: `tests/esp32_hall_test/TEST_RESULTS.md`

---

## Test 3 – ESP32 + ERTE RF

**Location:** `tests/esp32_rf_test/`

### Objective

Verify that ESP32 can generate the already proven ERTE FT45E RF protocol used by ControlRoll.

### Hardware

- ESP32-DevKit 38-pin
- 3-pin 433 MHz ASK/OOK transmitter: VCC / DATA / GND
- DATA on ESP32 GPIO25

### Test sequence

1. DOWN
2. STOP
3. UP
4. Repeat UP/STOP/DOWN 10 times each.
5. Restart ESP32.
6. Repeat UP/STOP/DOWN.
7. Test rapid command sequence.

### Acceptance

- all commands work
- no missed commands in repeated test
- reset does not affect operation
- rapid command sequence remains correct
- no unexpected movement
- proven RF payload/timing remains unchanged

### Result

**PASS – ESP32 RF subsystem accepted.**

Detailed results: `tests/esp32_rf_test/TEST_RESULTS.md`

---

# Test 4 – ESP32 + Hall + RF

**Location:** `tests/esp32_hall_rf_test/`

### Objective

Verify that Hall quadrature decoding and ERTE RF transmission operate simultaneously on one ESP32 without interfering with each other.

### Test sequence

1. Start ESP32 with both subsystems active.
2. Verify stable Hall state while stationary.
3. Move roller by RF command.
4. Observe Hall transitions during motor movement.
5. Stop by RF command.
6. Move in the opposite direction.
7. Repeat several cycles.
8. Include rapid RF commands.
9. Reset ESP32 and repeat.

### Acceptance

- RF commands remain reliable
- Hall direction remains correct while RF is active
- no invalid Hall transitions caused by RF activity
- no false CW/CCW changes
- relative position follows Hall transitions
- reset starts decoding correctly from the actual Hall state

**Status: NOT YET TESTED**

---

# Test 5 – ESP32 + Hall + RF + sensors

### Objective

Add the remaining sensors without disturbing the already verified Hall/RF functions.

Planned sensors:

- BH1750 – light level
- DS18B20 – temperature

### Test sequence

1. Run Hall/RF baseline.
2. Add BH1750.
3. Verify Hall/RF again.
4. Add DS18B20.
5. Verify Hall/RF again.
6. Run all sensors continuously while operating the blind.

### Acceptance

- no Hall errors
- no RF command failures
- sensor readings stable
- no blocking or timing problem caused by sensor handling

**Status: PLANNED**

---

# Test 6 – ESP32 + complete SmartRoll firmware

### Objective

Integrate all intended functions in one ESP32 firmware:

```text
                ┌── Hall A/B
                │
ESP32 ──────────┼── ERTE RF
                │
                ├── BH1750
                │
                ├── DS18B20
                │
                └── Wi-Fi / MQTT / Home Assistant
```

### Test areas

- manual RF control
- Hall-based movement/direction tracking
- temperature monitoring
- light monitoring
- Wi-Fi/MQTT communication
- Home Assistant commands
- simultaneous operation
- reboot/reconnect behaviour

**Status: PLANNED**

---

# Test 7 – Power supply and complete hardware

### Objective

Validate the final SmartRoll hardware, including the selected 230 V AC → low-voltage power supply/step-down solution.

### Test areas

- startup from mains
- ESP32 boot reliability
- 3.3 V stability
- RF operation
- Hall operation
- sensor operation
- thermal behaviour
- restart after power interruption

**Important:** mains testing is performed only after the low-voltage electronics have passed all previous tests.

**Status: PLANNED**

---

# Test 8 – Long-duration / final reliability test

### Objective

Operate a complete SmartRoll unit for an extended period and verify that no cumulative software or hardware fault develops.

### Test areas

- repeated blind movements
- repeated RF commands
- Hall position tracking
- Wi-Fi reconnects
- ESP32 resets
- sensor operation
- long-term temperature
- unexpected direction changes

**Status: PLANNED**

---

# Test status overview

| Test | System | Status |
|---|---|---|
| 1 | Hall hardware + geometry | PASS |
| 2 | ESP32 + Hall A/B | PASS |
| 3 | ESP32 + ERTE RF | PASS |
| 4 | ESP32 + Hall + RF | NOT TESTED |
| 5 | Hall + RF + BH1750 + DS18B20 | PLANNED |
| 6 | Complete SmartRoll firmware | PLANNED |
| 7 | Final hardware + 230 V supply | PLANNED |
| 8 | Long-duration reliability | PLANNED |

## Important design decisions carried through all tests

### Position after power failure

SmartRoll does **not** require absolute roller position recovery after a power failure. The motor's own upper and lower electronic limits remain the authoritative physical limits. SmartRoll tracks relative movement/direction and can resynchronize during a complete opening or closing cycle.

### Platform

The development tests use ESP32 because the final SmartRoll controller will also use ESP32. The Arduino Nano was used for the initial Hall/RF proof-of-concept only.

### RF protocol

The ERTE RF payload and timing validated in ControlRoll and Test 3 are frozen. Future integration tests must not modify the RF protocol unless a separate investigation is explicitly opened.
