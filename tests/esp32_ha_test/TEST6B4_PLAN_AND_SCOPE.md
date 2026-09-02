# SmartRoll – Test 6B.4 – MQTT Cover v Home Assistant

**Status: PLANNED / PARTIAL – Cover entity to be prepared**

## Purpose

Test 6B.4 completes the laboratory Home Assistant integration only to the level that is meaningful on the current Merkur mechanical test fixture.

The goal is to prepare a Home Assistant MQTT Cover entity using the already validated SmartRoll MQTT command interface. The laboratory fixture is **not** used for calibration of the real roller-blind position.

## Current laboratory setup

The SmartRoll electronics and firmware are being tested on a mechanical test fixture assembled from Merkur components. This fixture is suitable for verification of communication, RF commands, Hall sensing, direction detection and relative pulse counting, but it is not the final mechanical construction of the roller blind controller.

## Scope of 6B.4

The following functionality may be prepared and tested:

- Home Assistant MQTT Cover entity.
- Open command: `UP`.
- Close command: `DOWN`.
- Stop command: `STOP`.
- Existing MQTT command topic:

```text
smartroll/smartroll_01/cover/command
```

- Verification that the Cover uses the already validated ESP32 firmware and RF path.
- Preservation of the already tested Hall A/B, direction, relative position, temperature and light functions.

## Explicitly out of scope

Absolute roller-blind position calibration is **not** performed during 6B.4.

In particular, the following values are not to be established on the Merkur fixture:

- real CLOSED position,
- real OPEN position,
- number of Hall pulses between real roller-blind end positions,
- 0–100 % absolute position mapping,
- automatic positioning to a percentage,
- final end-stop calibration.

The pulse count observed on the Merkur fixture is only a laboratory relative-motion measurement. It must not be used as the calibration value for the final roller blind.

## Reason for postponing calibration

The absolute position depends on the actual roller-blind mechanism, motor/drive transmission, mechanical mounting, travel and end positions. A calibration performed on the Merkur fixture would therefore have no reliable meaning for the final installation.

## Firmware principle

The validated SmartRoll firmware and already tested elements are not to be changed merely to implement 6B.4. The existing RF, Hall, sensor, Wi-Fi and MQTT functionality remains the reference implementation.

The previously validated command path is:

```text
Home Assistant
      ↓ MQTT
Mosquitto
      ↓
ESP32 SmartRoll
      ↓ RF 433 MHz
ERTE receiver
      ↓
Laboratory mechanical test fixture
```

## Completion criterion

6B.4 is considered complete for the laboratory phase when the MQTT Cover entity has been prepared and its UP / STOP / DOWN commands have been verified without requiring absolute-position calibration.

## Transition to mechanical development

After the laboratory Cover integration, development moves to the mechanical part of SmartRoll:

1. finalize all mechanical components,
2. finalize the complete mechanical construction,
3. integrate the electronics and Hall/magnet arrangement into the final construction,
4. prepare the assembly for installation on the real roller blind,
5. only then begin phase **6C – installation and calibration on the real roller blind**.

Phase 6C is therefore intentionally postponed until the complete real mechanical construction is ready.

## Relation to previous tests

The MQTT command path was already verified in Test 6B.2 using the correct topic `smartroll/smartroll_01/cover/command`. UP, STOP and DOWN were all successfully tested, and firmware 6A.3 was not modified during that correction. See `TEST6B2_RESULT.md`.

Test 6B.3 subsequently verified control from Home Assistant together with monitoring of Hall direction, Hall position, temperature and light on the laboratory setup.

## Decision

**6B laboratory development ends without absolute-position calibration.**

The next major development activity is the mechanical construction for the actual roller-blind installation. Absolute position calibration belongs to phase 6C and will be performed only on the real roller blind.
