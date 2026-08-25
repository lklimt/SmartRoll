# SmartRoll – Purchase List No. 2

**Purpose:** Complete the material needed to perform the remaining SmartRoll tests and, if successful, assemble one complete functional SmartRoll prototype.

**Date:** 2026-08-25

## 1. Controller decision

The project has frozen the existing ControlRoll ERTE RF protocol as the reference for SmartRoll migration.

The RF433send library explicitly supports **ESP32, ESP8266 and AVR**, so the RF protocol can be migrated without redesigning the protocol itself. The remaining question is timing reliability when RF, Hall quadrature and Wi-Fi operate simultaneously.

The purchase baseline is therefore **ESP32 as the preferred final controller**.

The existing Wemos D1 mini / ESP8266 remains useful as development/fallback hardware and does not need to be discarded.

## 2. What is already available

Purchase No. 1 has already covered the initial Hall experiment material:

- Hall sensors – already purchased
- magnets – already purchased
- resistors – already purchased
- capacitors – already purchased
- universal prototype PCB – already available
- Merkur test fixture – already available
- Arduino Nano – available as the current Hall test platform
- Wemos D1 mini – existing development hardware

These items are therefore not required for the new purchase, except where a spare is useful.

## 3. Main controller – PREFERRED PLATFORM

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 2 | ESP32-DevKitC 38pin, Micro USB | Drátek | 1× main development/test unit, 1× final/reserve unit |

Current candidate: **ESP32-DevKitC 38pin**. Drátek lists the board with ESP32 dual-core, 4 MB Flash, Wi-Fi/Bluetooth, 3.3 V logic and 5 V USB power. urlDrátek – ESP32-DevKitC 38pinhttps://dratek.cz/arduino-platforma/51547-esp32-devkitc-development-board-38pin.html

Two boards are recommended so that one can remain permanently available as the development/debug unit while the second becomes the basis of the complete SmartRoll prototype.

## 4. RF control

The final SmartRoll shall generate the already validated ControlRoll ERTE RF protocol directly from the ESP32.

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 2 | NiceRF STX882 433 MHz ASK transmitter | Drátek | 1× test, 1× final/reserve |
| 2 | 433 MHz antenna suitable for STX882 / 433.92 MHz | Drátek | RF test and final unit |
| 1 | NiceRF SRX882S receiver | Drátek | optional RF reception/diagnostic testing |

The existing ControlRoll RF protocol is frozen in:

`docs/11_controlroll_rf_protocol.md`

The protocol shall not be changed during migration. The first task is to reproduce the existing waveform on ESP32 and verify the ERTE motor responds identically.

The STX882 is the preferred transmitter candidate. The RF module electrical interface must still be verified before final PCB design.

## 5. Light sensor

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 1 | BH1750 light intensity sensor | Drátek | digital lux measurement via I2C |

BH1750 provides I2C light measurement. urlDrátek – BH1750 light sensorhttps://dratek.cz/arduino-platforma/902-mereni-intenzity-svetla-bh1750.html

## 6. Temperature sensor

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 2 | Waterproof DS18B20, 1 m cable | Drátek | 1× test, 1× final/reserve |

The DS18B20 is suitable for the planned 1-Wire temperature measurement. urlDrátek – waterproof DS18B20 1 mhttps://dratek.cz/arduino-platforma/848-vodeodolny-teplomer-pro-jednodeskove-pocitace-ds18b20-1m.html

## 7. PIR motion sensor – NOT PURCHASED

PIR is **removed from SmartRoll v1**.

Hall A/B already provide the required information about actual roller-shaft movement and direction. A PIR detects movement/presence of people in the room, which is a separate future automation function and is not required for roller control.

No PIR is therefore included in Purchase List No. 2.

If presence-based automation is later approved, PIR will be treated as an independent optional expansion and purchased separately.

## 8. Power supply

For the integrated prototype, use a safe low-voltage 5 V DC supply during development.

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 1 | OMCH DR-15-5, 5 V / 3 A / 15 W DIN supply | LaskaKit | complete prototype power source |

urlLaskaKit – OMCH DR-15-5 5 V / 3 Ahttps://www.laskakit.cz/omch-dr-15-5-modulovy-napajeci-230v-ac-dc-zdroj-na-din-5v-3a-15w/

An alternative 5 V / 4 A supply may be used if the mechanical arrangement makes it more suitable.

Mains wiring is outside the low-voltage test phase and must be separately protected and documented.

## 9. Prototyping accessories

| Qty | Component | Purpose |
|---:|---|---|
| 1 | Dupont jumper assortment M-M / M-F / F-F | prototyping |
| 1 | 2.54 mm pin-header assortment | PCB/module connections |
| 1 | 2-pin / 3-pin screw terminal assortment | permanent prototype connections |
| 1 | hookup-wire assortment | integrated prototype |
| 1 | heat-shrink assortment | insulation/strain relief |
| 1 | spare prototype PCB, if required | integrated prototype |

## 10. Recommended electrical spares

| Qty | Component | Purpose |
|---:|---|---|
| 5 | 100 nF ceramic capacitors | module decoupling |
| 5 | 10 µF electrolytic capacitors | local supply stabilization |
| 2 | 470 µF electrolytic capacitors | supply/RF transient reserve |
| 5 | 1 kΩ resistors | signal/general use |
| 5 | 4.7 kΩ resistors | I2C / 1-Wire pull-up/general use |
| 5 | 10 kΩ resistors | pull-up/pull-down/general use |

These values are support/spare values and are not yet frozen production values.

## 11. ESP32 prototype GPIO baseline

The exact final GPIO allocation will be chosen after the ESP32 RF and Hall concurrency test.

Recommended initial allocation:

| Function | ESP32 GPIO | Status |
|---|---:|---|
| Hall A | GPIO32 | PROPOSED |
| Hall B | GPIO33 | PROPOSED |
| BH1750 SDA | GPIO21 | PROPOSED |
| BH1750 SCL | GPIO22 | PROPOSED |
| RF transmitter | GPIO25 | PROPOSED |
| DS18B20 | GPIO26 | PROPOSED |
| Light / ADC spare | GPIO34 | PROPOSED INPUT ONLY |

These GPIOs are deliberately selected from ordinary ESP32 pins suitable for the intended functions. The final pinout shall be frozen only after the actual modules are tested.

## 12. Test sequence enabled by Purchase No. 2

### Test 1 – Hall

Already completed on Arduino Nano. The quadrature algorithm and reset behavior are experimentally validated.

### Test 2 – ESP32 Hall decoder

Port the approved Arduino quadrature decoder to ESP32.

Acceptance:

- CW correct,
- CCW correct,
- rapid rotation correct,
- reset recovery correct,
- no false direction changes.

### Test 3 – ESP32 RF

Port the frozen ControlRoll RF protocol without changing its data or timing reference.

Acceptance:

- UP works,
- STOP works,
- DOWN works,
- correct roller addressing is preserved,
- repeated transmissions are reliable.

### Test 4 – ESP32 + Hall + RF simultaneously

This is the key architecture test.

Acceptance:

- no lost Hall transitions,
- no false direction changes,
- no unexpected invalid transitions,
- no RF command corruption,
- no watchdog/reset.

### Test 5 – Sensors

Add:

- BH1750,
- DS18B20.

Verify simultaneous operation and Home Assistant/MQTT reporting.

### Test 6 – Complete SmartRoll prototype

Combine:

```text
ESP32
+ Hall A/B
+ RF TX
+ BH1750
+ DS18B20
+ Wi-Fi/MQTT
```

Only after Tests 2–5 pass should the integrated controller be considered ready for mechanical installation.

## 13. Items intentionally NOT purchased yet

Do not buy these in quantity until the integrated tests are complete:

- custom SmartRoll PCB,
- final enclosure,
- production mains PCB,
- large quantity of Hall sensors/magnets,
- final RF antenna type/length,
- production connectors,
- final PCB protection components,
- additional level shifters unless testing proves one is required,
- PIR/presence sensor hardware.

## 14. Purchase priority

### MUST HAVE

1. **ESP32-DevKitC 38pin – 2 pcs**
2. **STX882 433 MHz transmitter – 2 pcs**
3. **433 MHz antennas – 2 pcs**
4. **BH1750 – 1 pc**
5. **DS18B20 waterproof – 2 pcs**
6. jumper wires / headers / terminals
7. support capacitors/resistors

### RECOMMENDED

8. SRX882S receiver – 1 pc
9. 5 V / 3 A power supply – 1 pc
10. spare prototype PCB
11. heat-shrink and hookup wire

## 15. Procurement rule

Preferred suppliers:

- **Drátek.cz**
- **LaskaKit.cz**

Availability and prices should be checked again immediately before ordering.

Any substitution with a different electrical interface shall be recorded in SmartRoll documentation before use.
