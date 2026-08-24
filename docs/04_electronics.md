# SmartRoll – Electronics

**Status:** Architecture approved; component-level details OPEN

SmartRoll shall use **one Wemos D1 mini / ESP8266** as the production controller for one external roller blind with an ERTE ET 45E motor.

## 1. Functional Hardware

The controller is intended to combine:

- two Hall sensors for quadrature rotation/direction detection,
- two magnets on the rotating carrier,
- 433 MHz RF transmitter for UP/STOP/DOWN control,
- motion sensor,
- temperature sensor,
- light sensor,
- Wi-Fi / Home Assistant communication.

The Arduino Nano used during Hall testing is development hardware only and is not part of the production architecture.

## 2. Initial GPIO Allocation

| Function | Wemos | ESP GPIO | Status |
|---|---|---:|---|
| Hall A | D1 | GPIO5 | DECIDED |
| Hall B | D2 | GPIO4 | DECIDED |
| Motion | D5 | GPIO14 | DECIDED |
| Temperature | D6 | GPIO12 | DECIDED |
| RF transmitter | D7 | GPIO13 | DECIDED |
| Light sensor | A0 | ADC | DECIDED |
| Spare | D0 | GPIO16 | RESERVED |
| Boot-sensitive | D3 | GPIO0 | RESERVED |
| Boot-sensitive / serial TX | D4 | GPIO2 | RESERVED |
| Boot-sensitive | D8 | GPIO15 | RESERVED |

ESP8266 boot mode depends on GPIO0, GPIO2 and GPIO15; therefore SmartRoll shall not assign ordinary sensor functions to these pins without an explicit electrical boot analysis.

## 3. Hall Interface

Hall A/B use the approved SmartRoll quadrature arrangement. The ESP firmware must implement the validated state-transition decoder.

```text
CW : 11 → 10 → 00 → 01 → 11
CCW: 11 → 01 → 00 → 10 → 11
```

Hall input electrical compatibility with 3.3 V ESP8266 GPIOs must be confirmed against the final Hall sensor.

## 4. RF Interface

The former ControlRoll system used an Arduino Nano to drive the 433 MHz transmitter. SmartRoll will move the RF generation into the ESP8266.

Initial RF output assignment:

```text
Wemos D7 / GPIO13 → RF transmitter DATA
```

The exact RF transmitter module and its 3.3 V/5 V data compatibility remain OPEN.

The existing ControlRoll RF protocol and per-roller codes are the starting reference and must be migrated only after verification.

## 5. Sensors

### Motion

Initial allocation: D5 / GPIO14.

Exact sensor and active level: OPEN.

### Temperature

Initial allocation: D6 / GPIO12.

A 1-Wire sensor is the current architectural assumption. Exact component and pull-up: OPEN.

### Light

Initial allocation: A0.

The final sensor/divider must be designed so the Wemos D1 mini ADC input remains within its specified range.

## 6. Timing Requirement

The Hall decoder must remain responsive during RF transmission.

Hall processing shall be interrupt/event driven or otherwise guaranteed not to miss valid transitions.

The ESP RF implementation shall not use long blocking operations that can prevent Hall processing, Wi-Fi servicing or watchdog servicing.

This interaction is a mandatory integration test before production firmware approval.

## 7. Power and Logic Levels

All external interfaces shall be checked for:

- supply voltage,
- logic-high and logic-low levels,
- maximum GPIO voltage,
- pull-up/pull-down requirements,
- startup state,
- current consumption.

The old ControlRoll RF hardware used 5 V supply on the Arduino side. Direct connection to ESP8266 GPIO must therefore not be assumed safe until the RF module data input/output levels are verified.

## 8. Detailed Component Selection – OPEN

- Hall sensor part number,
- RF transmitter module,
- RF receiver requirement,
- motion sensor,
- temperature sensor,
- light sensor,
- power supply,
- protection and filtering,
- final PCB implementation.

See [`docs/09_system_architecture.md`](09_system_architecture.md) for the complete single-ESP architecture and GPIO feasibility analysis.
