# SmartRoll

Automated control system for external roller blinds with **ERTE ET 45E tubular motors**.

SmartRoll combines the previously developed ControlRoll RF control with Hall-based direction/position sensing and additional environmental sensors in **one Wemos D1 mini / ESP8266 controller**.

## Project documentation

- [Technical Specification](SPECIFICATION.md)
- [System Architecture and D1 feasibility](docs/09_system_architecture.md)
- [Mechanical Design](docs/02_mechanical_design.md)
- [Hall Sensor Specification](docs/03_hall_sensor.md)
- [Electronics](docs/04_electronics.md)
- [Change Log](CHANGELOG.md)

## Target single-ESP architecture

One Wemos D1 mini / ESP8266 is planned to handle:

- Hall A + Hall B quadrature direction sensing
- relative roller position counting
- 433 MHz RF UP / STOP / DOWN control
- motion sensor
- temperature sensor
- light sensor
- Wi-Fi / Home Assistant communication

The Arduino Nano remains a **development/test platform only**.

## Initial GPIO allocation

| Function | Wemos pin | ESP GPIO |
|---|---|---:|
| Hall A | D1 | GPIO5 |
| Hall B | D2 | GPIO4 |
| Motion | D5 | GPIO14 |
| Temperature | D6 | GPIO12 |
| RF transmitter | D7 | GPIO13 |
| Light | A0 | ADC |

D3/GPIO0, D4/GPIO2 and D8/GPIO15 are reserved because they are ESP8266 boot-sensitive pins.

## Approved Hall arrangement

```text
2 magnets, 180° apart
Hall A + Hall B directly beside each other
5 mm successful test air gap

CW : 11 → 10 → 00 → 01 → 11
CCW: 11 → 01 → 00 → 10 → 11
```

The quadrature decoder and restart direction recovery have been experimentally validated on the Arduino Nano test fixture.

## Relationship to ControlRoll

The previous [ControlRoll](https://github.com/lklimt/ControlRoll) project used a Wemos D1 together with a separate Arduino Nano for RF transmission. SmartRoll is the next architecture: RF generation is moved into the ESP8266 so the production controller does not require the Arduino Nano.

The existing ControlRoll RF protocol and roller command codes are retained as the reference for the RF migration and must be verified before production firmware release.

## Repository structure

```text
SmartRoll/
├── README.md
├── SPECIFICATION.md
├── CHANGELOG.md
├── docs/
│   ├── 01_project_overview.md
│   ├── 02_mechanical_design.md
│   ├── 03_hall_sensor.md
│   ├── 04_electronics.md
│   ├── 05_firmware.md
│   ├── 06_home_assistant.md
│   ├── 07_testing.md
│   ├── 08_manufacturing.md
│   └── 09_system_architecture.md
├── mechanical/
│   ├── freecad/
│   │   ├── macros/
│   │   ├── models/
│   │   └── references/
│   ├── stl/
│   └── drawings/
├── electronics/
│   ├── schematics/
│   └── pcb/
├── firmware/
├── measurements/
├── tests/
└── images/
```

The technical specification is the source of truth for approved project parameters. Experimental results are retained as evidence but do not override the approved specification.
