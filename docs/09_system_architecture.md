# SmartRoll – System Architecture

**Status:** APPROVED ARCHITECTURE BASELINE / implementation OPEN

## 1. Objective

SmartRoll shall combine the functions previously developed in the separate `ControlRoll` project with the new SmartRoll sensing and position functions.

The target is **one Wemos D1 mini / ESP8266 per roller-blind controller**.

The controller shall provide:

- RF control of the ERTE ET 45E roller-blind motor,
- Hall A/B rotation and direction sensing,
- relative position counting,
- motion sensing,
- temperature sensing,
- light sensing,
- Wi-Fi communication,
- Home Assistant integration.

The previous ControlRoll architecture used a Wemos D1 communicating with a separate Arduino Nano which generated the RF signal. The new architecture removes that Arduino from the production system. The ESP8266 shall generate the RF output directly.

## 2. Feasibility Result

**Result: FEASIBLE on one Wemos D1 mini / ESP8266.**

The planned functions require only a small subset of the available GPIO resources.

The Wemos D1 mini exposes the ESP8266 GPIOs through the D0–D8 labels, plus A0. The approved design deliberately avoids using the boot-strap pins GPIO0, GPIO2 and GPIO15 for sensors whose startup state could interfere with boot.

ESP8266 boot mode depends on GPIO0, GPIO2 and GPIO15, so these pins shall be treated as reserved/boot-sensitive in the SmartRoll design.

## 3. Proposed GPIO Allocation

| Function | Wemos label | ESP8266 GPIO | Direction | Status |
|---|---|---:|---|---|
| Hall A | D1 | GPIO5 | input | DECIDED FOR ARCHITECTURE |
| Hall B | D2 | GPIO4 | input | DECIDED FOR ARCHITECTURE |
| Motion sensor | D5 | GPIO14 | input | DECIDED FOR ARCHITECTURE |
| Temperature sensor | D6 | GPIO12 | input / 1-Wire | DECIDED FOR ARCHITECTURE |
| RF transmitter | D7 | GPIO13 | output | DECIDED FOR ARCHITECTURE |
| Light sensor | A0 | ADC | analog input | DECIDED FOR ARCHITECTURE |
| D0 | D0 | GPIO16 | reserved | AVAILABLE / RESERVED |
| D3 | D3 | GPIO0 | boot-sensitive | RESERVED |
| D4 | D4 | GPIO2 | boot-sensitive / serial TX | RESERVED |
| D8 | D8 | GPIO15 | boot-sensitive | RESERVED |
| RX | RX | GPIO3 | serial RX | RESERVED / DEBUG |
| TX | TX | GPIO1 | serial TX | RESERVED / DEBUG |

This allocation leaves D0 unused and keeps D3/D4/D8 out of normal sensor assignments.

## 4. Hall Sensors

Hall A and Hall B use the already experimentally validated quadrature arrangement.

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

Hall inputs must be electrically compatible with the ESP8266 3.3 V GPIOs. Pull-up strategy shall be finalized with the selected Hall sensor.

## 5. RF Control

The existing ControlRoll project uses a 433 MHz transmitter and receiver with the Arduino Nano. The Arduino transmitter output was D4 and the RF timing implementation used the `RF433send` library.

In SmartRoll the production ESP shall generate the RF transmitter signal directly on **D7 / GPIO13**, subject to validation of the RF transmitter module and the ESP-compatible timing implementation.

The old Arduino Nano is therefore **not required in the production architecture**.

The existing ControlRoll command set remains the functional starting point:

```text
UP / STOP / DOWN
```

The existing project also contains per-roller RF command codes for rollers 1–6. Those codes shall be migrated into SmartRoll only after their source data is verified and documented.

### RF receiver

The previous ControlRoll hardware included a 433 MHz receiver, but the documented working control path is transmission from the Arduino to the roller motor. The SmartRoll first production architecture therefore does **not require a dedicated RF receiver GPIO**.

If later testing proves that RF reception is required, the spare GPIO resources shall be reassessed rather than assigning a boot-sensitive pin without verification.

## 6. Motion Sensor

A digital motion sensor may be connected to **D5 / GPIO14**.

The exact sensor part number and active level remain OPEN.

The firmware shall treat motion sensing as an independent input service and shall not block Hall decoding or RF timing.

## 7. Temperature Sensor

A 1-Wire temperature sensor is allocated to **D6 / GPIO12** as the initial architecture.

The exact sensor type and pull-up value remain OPEN until the component is finalized.

The temperature service shall be non-blocking from the perspective of Hall direction detection and RF control.

## 8. Light Sensor

The light sensor is allocated to **A0**.

The exact sensor implementation remains OPEN. If an LDR/resistor divider is used, its voltage range must remain within the actual Wemos D1 mini A0 input specification.

The firmware shall convert the raw ADC value to a calibrated light level only after the sensor is experimentally calibrated.

## 9. Wi-Fi and Home Assistant

The ESP8266 provides Wi-Fi directly, so no additional communication processor is required.

The production controller shall expose at least:

- roller state,
- requested movement,
- physical rotation direction,
- relative position/count,
- motion state,
- temperature,
- light level,
- diagnostics/invalid Hall transitions,
- communication/availability state.

The exact MQTT / ESPHome / native API architecture remains OPEN and shall be decided after the sensor and firmware architecture is frozen.

## 10. Firmware Architecture

The firmware shall be modular even though it runs on one ESP:

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

No module shall contain long blocking delays that can prevent Hall transitions from being processed.

## 11. Critical Timing Requirement

The Hall decoder and RF transmitter are the two timing-sensitive functions.

The Hall inputs shall be handled using GPIO interrupts or an equally reliable event-driven method on the ESP8266.

RF transmission shall be implemented so that its timing does not cause missed Hall transitions. This must be verified experimentally before the production firmware is accepted.

The fact that the D1 has enough GPIOs does **not by itself prove timing compatibility**. RF/Hall concurrency is therefore an explicit firmware test requirement.

## 12. Power and Logic Levels

The ESP8266 operates at 3.3 V logic.

Every external sensor and RF module must therefore be checked for:

- supply voltage,
- GPIO high/low levels,
- maximum ESP GPIO voltage,
- required pull-ups/pull-downs,
- startup state.

The existing ControlRoll RF modules were powered from 5 V in the Arduino system. Their direct electrical compatibility with ESP8266 GPIO must be verified before connecting the RF data input/output directly.

## 13. Production vs Development Hardware

### Development

- Arduino Nano: Hall algorithm and quadrature validation platform.
- Wemos D1: target ESP platform.
- Merkur fixture: mechanical Hall test platform.
- Existing ControlRoll hardware: RF protocol reference.

### Production

- one Wemos D1 mini / ESP8266,
- two Hall sensors,
- two magnets,
- RF transmitter,
- motion sensor,
- temperature sensor,
- light sensor.

The Arduino Nano is not part of the production controller.

## 14. Feasibility Conclusion

The single-Wemos architecture is accepted as the project direction.

There are sufficient GPIO resources for all currently planned functions, and the proposed allocation avoids the ESP8266 boot-sensitive GPIO0/GPIO2/GPIO15 pins.

The remaining feasibility risks are **not GPIO count**. They are:

1. direct ESP8266 RF timing compatibility,
2. RF module electrical level compatibility,
3. Hall interrupt reliability while RF is transmitting,
4. exact sensor component selection and electrical interface,
5. final power-supply design.

These items shall be verified before the first production firmware release.

## 15. Open Items

- final Wemos D1 variant,
- exact Hall sensor part number,
- exact motion sensor,
- exact temperature sensor,
- exact light sensor,
- RF transmitter module and electrical interface,
- whether an RF receiver is ultimately required,
- RF timing implementation on ESP8266,
- final Home Assistant communication method,
- final PCB/GPIO electrical protection,
- power supply and decoupling.
