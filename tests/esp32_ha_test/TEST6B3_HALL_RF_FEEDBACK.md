# SmartRoll – Test 6B.3
## RF command + Hall feedback

**Status: READY FOR TEST**

## Objective

Verify that a command issued from Home Assistant causes the ERTE RF receiver to operate and that the resulting mechanical movement is simultaneously detected by the existing Hall quadrature sensor and reported back to Home Assistant.

**Important:** This test does not change the validated ESP32 firmware 6A.3. The existing RF, Hall, sensor, Wi-Fi and MQTT implementation remains unchanged.

## Verified starting point

- Test 6A: ESP32 sensors, Hall, RF, Wi-Fi and MQTT – PASS
- Test 6B.1: MQTT → Home Assistant sensor reporting – PASS
- Test 6B.2: Home Assistant → MQTT → ESP32 → ERTE RF commands – PASS

## MQTT topics

State:

```text
smartroll/smartroll_01/state
```

Command:

```text
smartroll/smartroll_01/cover/command
```

Availability:

```text
smartroll/smartroll_01/availability
```

Commands:

```text
UP
STOP
DOWN
```

## Test principle

```text
Home Assistant
      │
      │ UP / DOWN / STOP
      ▼
Mosquitto
      │
      ▼
ESP32
      │
      ├──────────────► RF433 ► ERTE receiver
      │                         │
      │                         ▼
      │                   laboratory mechanism
      │                         │
      │                         ▼
      └──────── Hall A/B ◄──── mechanical movement
                 │
                 ▼
              position
              direction
                 │
                 ▼
              MQTT state
                 │
                 ▼
          Home Assistant
```

## Important distinction

The current `position` value is a **relative quadrature counter**, not yet a calibrated 0–100 % blind position.

Therefore this test verifies movement feedback and direction. It does **not** yet define the final Home Assistant `cover` position model.

## Preparation

Use only the laboratory mechanical Hall setup and laboratory ERTE receiver.

Before starting:

- ESP32 firmware 6A.3 is running.
- Wi-Fi is connected.
- MQTT is connected.
- Home Assistant test entities from 6B.1 are available.
- HA UP/STOP/DOWN buttons from 6B.2 are available.
- MQTT Explorer may be used as an independent observer.
- Record the initial `position` value.

## Test 1 – UP with Hall feedback

1. Record the current HA `Position`.
2. Record `Direction`.
3. Press **UP** in Home Assistant.
4. Confirm the laboratory mechanism moves.
5. Observe Hall A/B in HA.
6. Observe `Position` changing.
7. Confirm `Direction` changes to the expected direction.
8. Press **STOP**.
9. Confirm the RF movement stops.
10. Confirm the Hall position remains stable after stopping.

## Test 2 – DOWN with Hall feedback

1. Record the current position.
2. Press **DOWN** in HA.
3. Confirm the laboratory mechanism moves in the opposite direction.
4. Observe Hall A/B.
5. Confirm `Position` changes in the opposite direction.
6. Confirm `Direction` changes accordingly.
7. Press **STOP**.
8. Confirm the position stops changing.

## Test 3 – rapid command sequence

Use the laboratory setup only.

```text
UP → STOP → DOWN → STOP → UP → STOP
```

Confirm that each RF command is executed and that Hall feedback follows the actual mechanical movement.

## Test 4 – MQTT observation

While performing the tests, MQTT Explorer should show:

```text
smartroll/smartroll_01/cover/command
```

for commands and:

```text
smartroll/smartroll_01/state
```

for feedback.

The state JSON should show changes in the Hall-related values while the mechanism moves.

## PASS criteria

- [ ] HA UP command operates the laboratory ERTE receiver.
- [ ] HA DOWN command operates the laboratory ERTE receiver in the opposite direction.
- [ ] HA STOP stops the RF-driven movement.
- [ ] Hall A/B change during mechanical movement.
- [ ] Position changes in response to movement.
- [ ] Direction corresponds to the previously validated Hall direction.
- [ ] Position stops changing after the mechanism stops.
- [ ] MQTT state reports the Hall changes.
- [ ] HA displays the changed Hall/position/direction values.
- [ ] No regression of temperature/light reporting.
- [ ] No firmware modification to the validated 6A.3 baseline is required.

## Evidence

Store evidence in:

```text
 tests/esp32_ha_test/evidence/
```

Recommended:

```text
esp32_ha_testB3_serial.txt
mqtt_explorer_B3.png
home_assistant_B3.png
```

## What this test does NOT establish

This test does not yet establish:

- calibrated 0–100 % position,
- end-stop detection,
- absolute position after power loss,
- final HA Cover entity,
- automatic MQTT Discovery,
- automatic stopping at a requested percentage.

Those functions belong to the subsequent finalization stages after movement feedback has been validated.
