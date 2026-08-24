# SmartRoll – System Architecture

**Status:** APPROVED ARCHITECTURE BASELINE / implementation OPEN

## 1. Objective

SmartRoll combines the functions previously developed in the separate `ControlRoll` project with the new SmartRoll sensing and position functions.

The preferred target is **one ESP32 development/controller board per roller blind**.

The controller shall provide:

- RF control of the ERTE ET 45E roller-blind motor,
- Hall A/B rotation and direction sensing,
- relative position counting,
- motion sensing,
- temperature sensing,
- light sensing,
- Wi-Fi communication,
- Home Assistant integration.

The previous ControlRoll architecture used a Wemos D1 communicating with a separate Arduino Nano which generated the RF signal. SmartRoll removes that Arduino from the production system. The ESP shall generate the RF output directly.

The existing Wemos D1 / ESP8266 remains a useful development and fallback platform.

## 2. Controller Decision

**ESP32 is now the preferred final controller platform.**

Reason:

- substantially more GPIO resources,
- greater processing and memory headroom,
- hardware peripherals suitable for timing-sensitive work,
- sufficient resources for Hall decoding, RF, Wi-Fi and all planned sensors simultaneously.

The project shall nevertheless verify RF + Hall + Wi-Fi concurrency before the controller platform is considered production-frozen.

## 3. Feasibility Result

**Result: FEASIBLE on one ESP32.**

The planned functions require only a small subset of normal ESP32 GPIO resources.

The ESP32 has sufficient GPIO and peripheral resources to avoid the GPIO compromises that were present on the ESP8266 design.

## 4. Proposed ESP32 GPIO Allocation

Initial development allocation:

| Function | ESP32 GPIO | Direction | Status |
|---|---:|---|---|
| Hall A | GPIO32 | input | PROPOSED |
| Hall B | GPIO33 | input | PROPOSED |
| BH1750 SDA | GPIO21 | I2C | PROPOSED |
| BH1750 SCL | GPIO22 | I2C | PROPOSED |
| RF transmitter | GPIO25 | output | PROPOSED |
| Motion PIR | GPIO27 | input | PROPOSED |
| DS18B20 | GPIO26 | 1-Wire | PROPOSED |
| Light / ADC spare | GPIO34 | input-only ADC | PROPOSED |

These pins are ordinary ESP32 GPIOs appropriate for the intended functions. The final pinout shall be frozen after module-level and concurrency testing.

## 5. Hall Sensors

Hall A and Hall B use the experimentally validated quadrature arrangement.

Approved mechanical arrangement:

- two magnets 180° apart,
- Hall A and Hall B directly beside each other,
- successful tested air gap 5 mm.

Approved state sequences:

```text
CW : 11 → 10 → 00 → 01 → 11
CCW: 11 → 01 → 00 → 10 → 11
```

The ESP firmware shall implement the same complete 2-bit state-transition decoder validated on the Arduino Nano.

Hall inputs must be electrically compatible with the ESP32 3.3 V GPIOs. Pull-up strategy shall be finalized with the selected Hall sensor.

## 6. RF Control – FROZEN PROTOCOL

The existing ControlRoll project contains the reverse-engineered and validated ERTE 433 MHz RF control protocol.

This protocol is now **FROZEN** as a SmartRoll reference.

The following shall not be changed during migration:

- ERTE command data,
- roller addressing,
- UP / STOP / DOWN semantics,
- TRIBIT encoding,
- validated RF timing parameters.

The implementation reference is documented in:

`docs/11_controlroll_rf_protocol.md`

The `RF433send` library explicitly supports AVR, ESP8266 and ESP32. SmartRoll shall first reproduce the existing protocol directly on ESP32 using the same library and parameters.

Only after successful physical verification may a hardware-timed alternative be considered.

## 7. RF + Hall Timing Requirement

The most important SmartRoll RF test is simultaneous operation:

```text
Hall A/B rotation
        +
433 MHz RF transmission
        +
Wi-Fi active
```

Acceptance requires:

- no missed Hall transitions,
- no false direction changes,
- no unexpected invalid transitions,
- no RF command corruption,
- no watchdog reset.

The existing RF library uses microsecond timing and therefore the concurrency test is mandatory.

## 8. Motion Sensor

A digital PIR motion sensor is allocated to GPIO27.

The exact sensor and active level remain OPEN until the purchased sensor is tested.

Motion processing shall never block Hall decoding or RF control.

## 9. Temperature Sensor

A DS18B20 1-Wire sensor is initially allocated to GPIO26.

The sensor pull-up and exact wiring shall be verified on the ESP32 prototype.

Temperature measurement shall be scheduled without blocking time-critical Hall processing.

## 10. Light Sensor

BH1750 is initially allocated to I2C:

```text
SDA = GPIO21
SCL = GPIO22
```

The final sensor implementation remains subject to physical testing and calibration.

If an analog light sensor is considered later, an appropriate ADC pin shall be selected instead.

## 11. Wi-Fi and Home Assistant

The ESP32 provides Wi-Fi directly.

The production controller shall expose at least:

- roller state,
- requested movement,
- physical rotation direction,
- relative position/count,
- motion state,
- temperature,
- light level,
- Hall invalid-transition diagnostics,
- communication/availability state.

The exact MQTT / ESPHome / native API architecture remains OPEN and will be selected after the firmware architecture is frozen.

## 12. Firmware Architecture

```text
SmartRoll firmware
│
├── hall
│   ├── Hall A/B input
│   └── quadrature decoder
│
├── roller
│   ├── direction
│   ├── relative position
│   ├── movement state
│   └── endpoint synchronization
│
├── rf
│   ├── UP
│   ├── STOP
│   └── DOWN
│
├── sensors
│   ├── motion
│   ├── temperature
│   └── light
│
├── network
│   └── Wi-Fi
│
└── integration
    └── Home Assistant
```

No module shall contain long blocking delays that can prevent Hall events from being processed.

## 13. Power and Logic Levels

The ESP32 uses 3.3 V logic.

Every external sensor and RF module shall be checked for:

- supply voltage,
- GPIO high/low levels,
- maximum ESP32 GPIO voltage,
- pull-ups/pull-downs,
- startup state.

No 5 V signal shall be connected directly to an ESP32 GPIO without verification of compatibility.

## 14. Development vs Production Hardware

### Development

- Arduino Nano: validated Hall algorithm test platform.
- Wemos D1 / ESP8266: existing ControlRoll and fallback development platform.
- ESP32: preferred SmartRoll integration/test platform.
- Merkur fixture: Hall mechanical test platform.
- Existing ControlRoll hardware: RF protocol reference.

### Production target

- one ESP32 controller,
- two Hall sensors,
- two magnets,
- RF transmitter,
- motion sensor,
- temperature sensor,
- light sensor.

The Arduino Nano is not part of the production controller.

## 15. Feasibility Conclusion

The one-controller architecture is accepted as the project direction.

ESP32 is the preferred controller because it gives sufficient GPIO and processing headroom without forcing sensors onto boot-sensitive pins.

The remaining feasibility risks are:

1. direct ESP32 RF timing compatibility,
2. RF module electrical level compatibility,
3. Hall interrupt reliability while RF is transmitting,
4. exact sensor component selection and electrical interface,
5. final power-supply design.

These shall be verified before the first production firmware release.

## 16. Open Items

- final ESP32 board variant,
- exact Hall sensor production part number,
- exact motion sensor,
- exact temperature sensor wiring,
- exact light sensor implementation,
- RF transmitter module and electrical interface,
- whether an RF receiver is ultimately required,
- RF + Hall concurrency result,
- final Home Assistant communication method,
- final PCB/electrical protection,
- final power supply and decoupling.
