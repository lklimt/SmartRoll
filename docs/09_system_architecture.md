# SmartRoll – System Architecture

**Status:** APPROVED ARCHITECTURE BASELINE / implementation OPEN

## 1. Objective

SmartRoll combines the functions previously developed in the separate `ControlRoll` project with the new SmartRoll sensing and position functions.

The preferred target is **one ESP32 development/controller board per roller blind**.

The controller shall provide:

- RF control of the ERTE ET 45E roller-blind motor,
- Hall A/B rotation and direction sensing,
- relative position counting,
- temperature sensing,
- light sensing,
- Wi-Fi communication,
- Home Assistant integration.

**PIR motion detection is not part of SmartRoll v1.** Hall A/B is the authoritative sensor for physical roller movement. A PIR, if required in the future, would detect people/presence in the room and would be an independent automation input, not a roller-motion sensor.

The previous ControlRoll architecture used a Wemos D1 communicating with a separate Arduino Nano which generated the RF signal. SmartRoll removes that Arduino from the production system. The ESP shall generate the RF output directly.

The existing Wemos D1 / ESP8266 remains a useful development and fallback platform.

## 2. Controller Decision

**ESP32 is now the preferred final controller platform.**

Reason:

- substantially more GPIO resources,
- greater processing and memory headroom,
- hardware peripherals suitable for timing-sensitive work,
- sufficient resources for Hall decoding, RF, Wi-Fi and all planned v1 sensors simultaneously.

The project shall nevertheless verify RF + Hall + Wi-Fi concurrency before the controller platform is considered production-frozen.

## 3. Feasibility Result

**Result: FEASIBLE on one ESP32.**

The planned v1 functions require only a small subset of normal ESP32 GPIO resources.

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
| DS18B20 | GPIO26 | 1-Wire | PROPOSED |
| Light / ADC spare | GPIO34 | input-only ADC | PROPOSED |

PIR is deliberately not assigned a GPIO in v1.

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

**Hall A/B is the authoritative source of roller shaft movement and direction.** PIR must never be used to determine whether the roller is moving.

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

## 8. Temperature Sensor

A DS18B20 1-Wire sensor is initially allocated to GPIO26.

The sensor pull-up and exact wiring shall be verified on the ESP32 prototype.

Temperature measurement shall be scheduled without blocking time-critical Hall processing.

## 9. Light Sensor

BH1750 is initially allocated to I2C:

```text
SDA = GPIO21
SCL = GPIO22
```

The final sensor implementation remains subject to physical testing and calibration.

If an analog light sensor is considered later, an appropriate ADC pin shall be selected instead.

## 10. Power Architecture – 230 VAC to Isolated 5 V DC

The production concept shall use a **compact isolated AC/DC module**, not a large DIN-rail supply.

Preferred candidate:

**Hi-Link HLK-PM01 – 100–240 VAC input → 5 V DC / 0.6 A / 3 W, isolated.**

The module is approximately 34 × 20 × 15 mm and is intended for embedded installation. The selected supplier documentation specifies an isolated output and recommends appropriate input protection such as fuse and varistor. citeturn0search0turn0search24

The proposed architecture is:

```text
230 VAC
   │
   ├── mains input protection
   │
   ▼
┌─────────────────┐
│    HLK-PM01     │
│ 230 VAC → 5 VDC │
│     0.6 A       │
│    isolated     │
└────────┬────────┘
         │ 5 V DC
         ▼
       ESP32
         │
    ┌────┼───────────────┐
    │    │       │       │
   Hall  RF    BH1750  DS18B20
```

### Power-supply status

HLK-PM01 is **PREFERRED CANDIDATE, NOT YET PRODUCTION-FROZEN**.

The 0.6 A continuous rating must be verified under representative worst-case operation:

- ESP32 Wi-Fi active,
- RF transmission,
- Hall processing,
- BH1750 active,
- DS18B20 conversion.

If the 5 V rail becomes unstable or the ESP32 reports brownouts, a higher-current compact isolated AC/DC module shall be selected.

The former 5 V / 3 A DIN-rail supply is therefore no longer the baseline purchase item, but remains a possible laboratory fallback.

### Mains safety

The AC/DC module is directly connected to hazardous mains voltage. The final SmartRoll PCB/enclosure must maintain the required separation between mains and SELV circuitry and provide suitable protection, insulation, terminals and strain relief.

The exact fuse, surge protection, creepage/clearance and enclosure arrangement remain OPEN until the final PCB and mechanical design. Mains wiring shall not be performed on the current low-voltage prototype board.

## 11. Wi-Fi and Home Assistant

The ESP32 provides Wi-Fi directly.

The production controller shall expose at least:

- roller state,
- requested movement,
- physical rotation direction,
- relative position/count,
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

## 13. PIR / Presence – FUTURE ONLY

PIR is intentionally excluded from SmartRoll v1.

If later added, its purpose would be **human movement/presence detection in the room**, for example to support comfort or energy-saving automation.

It would not replace or supplement the Hall-based roller movement measurement.

Any future PIR implementation shall be documented as an independent automation input.

## 14. Power and Logic Levels

The ESP32 uses 3.3 V logic.

The HLK-PM01 supplies 5 V to the ESP32 development board/prototype. The ESP32's onboard regulator then provides the required 3.3 V rail for the ESP32 and compatible sensors.

Every external sensor and RF module shall be checked for:

- supply voltage,
- GPIO high/low levels,
- maximum ESP32 GPIO voltage,
- pull-ups/pull-downs,
- startup state.

No 5 V signal shall be connected directly to an ESP32 GPIO without verification of compatibility.

## 15. Development vs Production Hardware

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
- temperature sensor,
- light sensor,
- compact isolated 230 VAC → 5 V DC power module.

The Arduino Nano is not part of the production controller.

## 16. Feasibility Conclusion

The one-controller architecture is accepted as the project direction.

ESP32 is the preferred controller because it gives sufficient GPIO and processing headroom without forcing sensors onto boot-sensitive pins.

The compact isolated HLK-PM01 power architecture is now the preferred power concept because it fits the expected low power demand and the physical constraints of an embedded SmartRoll controller better than a 15 W DIN-rail supply.

The remaining feasibility risks are:

1. direct ESP32 RF timing compatibility,
2. RF module electrical level compatibility,
3. Hall interrupt reliability while RF is transmitting,
4. exact sensor component selection and electrical interface,
5. HLK-PM01 0.6 A power margin under worst-case load,
6. final mains protection and PCB/enclosure safety.

These shall be verified before the first production firmware release.

## 17. Open Items

- final ESP32 board variant,
- exact Hall sensor production part number,
- exact temperature sensor wiring,
- exact light sensor implementation,
- RF transmitter module and electrical interface,
- whether an RF receiver is ultimately required,
- RF + Hall concurrency result,
- final Home Assistant communication method,
- final PCB/electrical protection,
- HLK-PM01 load test result,
- final mains protection and enclosure arrangement,
- whether a future PIR/presence feature is desired.
