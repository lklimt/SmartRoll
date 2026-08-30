# SmartRoll – Complete Test Sequence

This document is the master validation sequence for the SmartRoll project. Tests are performed incrementally so that each subsystem is proven independently before subsystems are combined.

## Principle

**Do not move to the next integration stage until the preceding stage has passed.**

The purpose is to isolate faults and avoid changing several variables at once.

---

## Test 1 – Hall sensor hardware and geometry

**Location:** `tests/hall_sensor_test/`

**Status: PASS – Hall hardware and final mechanical arrangement accepted.**

---

## Test 2 – ESP32 + Hall A/B quadrature

**Location:** `tests/esp32_hall_test/`

**Status: PASS – ESP32 Hall subsystem accepted.**

---

## Test 3 – ESP32 + ERTE RF

**Location:** `tests/esp32_rf_test/`

**Status: PASS – ESP32 RF subsystem accepted.**

---

# Test 4 – ESP32 + Hall + RF

**Location:** `tests/esp32_hall_rf_test/`

### Completed subtests

- **4A – stationary baseline: PASS**
- **4B – RF UP + Hall tracking: PASS**
- **4C – RF DOWN + Hall tracking: PASS**
- **4D – repeated UP/STOP/DOWN cycles: PASS**
- **4E – RF commands during Hall activity: PASS**
- **4F – ESP32 reset: PASS**

### Result

**PASS – Test 4 completed successfully.**

---

# Test 5 – ESP32 + Hall + RF + sensors

**Location:** `tests/esp32_sensors_test/`

### Completed subtests

- **5A – sensors alone: PASS**
- **5B – sensors + Hall: PASS**
- **5C – sensors + RF: PASS**
- **5D – all functions simultaneously: PASS**
- **5E – ESP32 reset: PASS**

### Result

**PASS – Test 5 completed successfully.**

---

# Test 6 – Complete SmartRoll firmware + Home Assistant

**Location:** `tests/esp32_complete_test/`

### Objective

Integrate all validated functions in one ESP32 application and connect it to Home Assistant through the existing Mosquitto MQTT broker.

```text
                    ESP32 SmartRoll
                          │
       ┌──────────────────┼──────────────────┐
       │                  │                  │
   Hall A/B             ERTE RF          Sensors
       │                  │             ┌───┴────┐
       │                  │             │BH1750  │
       │                  │             │DS18B20 │
       │                  │             │PIR     │
       └──────────────────┼─────────────┴────────┘
                          │
                       Wi-Fi
                          │
                      IoT network
                          │
                       MQTT
                          │
                  Home Assistant
```

### Firmware

`tests/esp32_complete_test/smartroll_test6.ino`

### Home Assistant integration

`tests/esp32_complete_test/HA_MQTT.md`

### Test stages

- **6A – complete local firmware:** NOT TESTED
- **6B – Home Assistant MQTT discovery:** NOT TESTED
- **6C – Home Assistant RF control:** NOT TESTED
- **6D – sensors + movement:** NOT TESTED
- **6E – Wi-Fi/MQTT reconnect:** NOT TESTED
- **6F – ESP32 reset:** NOT TESTED
- **6G – long combined operation:** NOT TESTED

### Important

The Test 6 firmware keeps the proven RF payload/timing and Hall transition decoder. The new elements are the application layer, Wi-Fi, MQTT and Home Assistant discovery.

Absolute blind position is intentionally not implemented yet because Hall-count-to-blind-position calibration remains OPEN.

**Status: IN PROGRESS – firmware prepared, physical/HA testing not yet completed.**

---

# Test 7 – Power supply and complete hardware

### Objective

Validate the final SmartRoll hardware, including the selected 230 V AC → low-voltage power supply/step-down solution.

**Status: PLANNED**

---

# Test 8 – Long-duration / final reliability test

### Objective

Operate a complete SmartRoll unit for an extended period and verify that no cumulative software or hardware fault develops.

**Status: PLANNED**

---

# Test status overview

| Test | System | Status |
|---|---|---|
| 1 | Hall hardware + geometry | PASS |
| 2 | ESP32 + Hall A/B | PASS |
| 3 | ESP32 + ERTE RF | PASS |
| 4 | ESP32 + Hall + RF | **PASS** |
| 5 | Hall + RF + BH1750 + DS18B20 | **PASS** |
| 6 | Complete SmartRoll + Home Assistant | **IN PROGRESS** |
| 7 | Final hardware + 230 V supply | PLANNED |
| 8 | Long-duration reliability | PLANNED |

## Important design decisions carried through all tests

### Position after power failure

SmartRoll does **not** require absolute roller position recovery after a power failure. The motor's own upper and lower electronic limits remain the authoritative physical limits. SmartRoll tracks relative movement/direction and can resynchronize during a complete opening or closing cycle.

### Platform

The development tests use ESP32 because the final SmartRoll controller will also use ESP32. The Arduino Nano was used for the initial Hall/RF proof-of-concept only.

### RF protocol

The ERTE RF payload and timing validated in ControlRoll and Test 3 are frozen. Future integration tests must not modify the RF protocol unless a separate investigation is explicitly opened.
