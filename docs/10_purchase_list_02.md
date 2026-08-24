# SmartRoll – Purchase List No. 2

**Purpose:** Complete the material needed to perform the remaining SmartRoll tests and, if successful, assemble one complete functional SmartRoll prototype on a single Wemos D1 mini / ESP8266.

**Date:** 2026-08-25

## 1. What is already available

Purchase No. 1 has already covered the initial Hall experiment material:

- Hall sensors – already purchased
- magnets – already purchased
- resistors – already purchased
- capacitors – already purchased
- universal prototype PCB – already available
- Merkur test fixture – already available
- Arduino Nano – available as the current Hall test platform

These items are therefore **not repeated in Purchase No. 2** unless a spare is useful.

## 2. Main controller

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 2 | WeMos D1 mini ESP8266 V2.0 | Drátek | 1× test/development, 1× reserve/final unit |

The D1 mini is an ESP8266 board with Wi-Fi, 11 digital I/O and one analog input. It operates with 3.3 V logic. urlDrátek – WeMos D1 mini ESP8266 V2.0https://dratek.cz/arduino-platforma/121932-wemos-d1-mini-esp8266-wifi-modul-v2.0.html

## 3. RF control

The final SmartRoll must replace the present ControlRoll Arduino RF transmitter arrangement with RF generated directly by the ESP8266.

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 2 | NiceRF STX882 433 MHz ASK transmitter | Drátek | 1× test, 1× final/reserve |
| 2 | 433 MHz antenna suitable for STX882 / 433.92 MHz | Drátek | reliable RF test and final unit |
| 1 | NiceRF SRX882S receiver | Drátek | RF reception/diagnostic testing |

The STX882 is preferred because it is specified for operation up to 3.6 V, making it appropriate for direct ESP8266 logic-level testing. Drátek lists the STX882 as a 433 MHz ASK transmitter. urlDrátek – NiceRF STX882 transmitter + SRX882S receiver sethttps://dratek.cz/arduino-platforma/3172-nicerf-vysilac-prijimac-2x-antena-set-4ks.html

**Important:** the final SmartRoll RF implementation must reproduce the already validated ControlRoll RF protocol. The RF module selection does not by itself prove protocol compatibility; this must be tested before the production PCB is designed.

The original ControlRoll uses an Arduino RF output on D4 and communicates from Wemos to Arduino at 115200 Bd. The objective of SmartRoll is to eliminate the Arduino from the final unit. fileciteturn65file0L2-L2

## 4. Light sensor

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 1 | BH1750 light intensity sensor | Drátek | digital lux measurement via I2C |

BH1750 operates from 3–5 V and provides I2C communication. urlDrátek – BH1750 light sensorhttps://dratek.cz/arduino-platforma/902-mereni-intenzity-svetla-bh1750.html

## 5. Temperature sensor

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 2 | Waterproof DS18B20, 1 m cable | Drátek | 1× test, 1× final/reserve |

The DS18B20 works from 3.0–5.5 V and provides ±0.5 °C accuracy in the specified range. urlDrátek – waterproof DS18B20 1 mhttps://dratek.cz/arduino-platforma/848-vodeodolny-teplomer-pro-jednodeskove-pocitace-ds18b20-1m.html

## 6. Motion sensor

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 2 | PIR HC-SR501 | Drátek | 1× test, 1× final/reserve |

The HC-SR501 has a 3.3 V logic output and accepts 4.5–20 V supply. urlDrátek – HC-SR501 PIR sensorhttps://dratek.cz/arduino-platforma/839-hc-sr501-pohybove-cidlo-pro-jednodeskove-pocitace.html

For the final ESP installation, the signal level and startup behavior must be verified before permanent wiring.

## 7. Power supply for complete prototype

For the first integrated prototype, use a safe 5 V DC supply rather than experimenting with mains directly on the prototype PCB.

For the eventual complete unit, a compact enclosed/module AC/DC supply can be used only after the mains section has been mechanically and electrically designed and verified.

### Preferred prototype/final candidate

| Qty | Component | Preferred source | Reason |
|---:|---|---|---|
| 1 | 230 VAC → 5 VDC / 3 A enclosed modular supply | LaskaKit | generous current reserve for ESP + sensors + RF |

Recommended candidate: **OMCH DR-15-5, 5 V / 3 A / 15 W, DIN mount**. It has overload, short-circuit and thermal protection. urlLaskaKit – OMCH DR-15-5 5 V / 3 Ahttps://www.laskakit.cz/omch-dr-15-5-modulovy-napajeci-230v-ac-dc-zdroj-na-din-5v-3a-15w/

Alternative for a compact non-DIN prototype: **LYONZG S-20-5, 5 V / 4 A / 20 W**. urlLaskaKit – LYONZG S-20-5 5 V / 4 Ahttps://www.laskakit.cz/lyonzg-s-20-5-modulovy-napajeci-230v-ac-dc-zdroj-5v-4a-20w/

**Safety:** mains wiring is not part of the low-voltage prototype tests and must be isolated from the ESP/sensor development area. Final mains construction shall be documented separately.

## 8. Prototyping and wiring accessories

| Qty | Component | Purpose |
|---:|---|---|
| 1 | Dupont jumper assortment M-M / M-F / F-F | prototyping |
| 1 | 2.54 mm pin-header assortment | PCB and module connections |
| 1 | 2-pin / 3-pin screw terminal assortment | permanent prototype connections |
| 1 | 2.54 mm stripboard/prototype PCB, if the current board is insufficient | integrated prototype |
| 1 | 433 MHz antenna wire / connector material | RF testing |
| 1 | small assortment of heat-shrink tubing | insulation and strain relief |
| 1 | small assortment of hookup wire | final prototype wiring |

## 9. Recommended electrical protection/spares

| Qty | Component | Purpose |
|---:|---|---|
| 5 | 100 nF ceramic capacitors | local module decoupling |
| 5 | 10 µF electrolytic capacitors | local supply stabilization |
| 2 | 470 µF electrolytic capacitors | supply/RF transient reserve |
| 5 | 1 kΩ resistors | signal/LED/general use |
| 5 | 4.7 kΩ resistors | I2C / 1-Wire pull-up and general use |
| 5 | 10 kΩ resistors | pull-ups/pull-downs |
| 2 | 1 A resettable fuse/PTC or suitable low-voltage protection element | prototype protection |

These are **spares/support components**, not yet frozen production values.

## 10. GPIO allocation for the integrated SmartRoll prototype

The architecture has been revised to account for the BH1750 I2C bus. Hall A/B must not occupy the I2C pins.

| Function | D1 mini pin | ESP GPIO | Status |
|---|---|---:|---|
| BH1750 SDA | D2 | GPIO4 | DECIDED |
| BH1750 SCL | D1 | GPIO5 | DECIDED |
| Hall A | D5 | GPIO14 | DECIDED |
| Hall B | D6 | GPIO12 | DECIDED |
| RF transmitter | D7 | GPIO13 | DECIDED |
| Motion PIR | D0 | GPIO16 | DECIDED for prototype; polling is acceptable |
| DS18B20 | D3 | GPIO0 | PROVISIONAL – boot behavior must be tested |
| A0 | A0 | ADC0 | RESERVED |
| D4 | D4 | GPIO2 | RESERVED |
| D8 | D8 | GPIO15 | RESERVED |
| RX/TX | RX/TX | GPIO3/GPIO1 | RESERVED for programming/diagnostics |

The D1 mini pinout and ESP8266 GPIO mapping are documented by the manufacturer schematic. urlWeMos D1 mini schematichttps://www.laskakit.cz/user/related_files/wemos_d1_mini_schematic.pdf

The ESP8266 uses 3.3 V logic; 5 V signals must not be connected directly to its GPIO. urlDrátek – WeMos D1 guidancehttps://navody.dratek.cz/navody-k-produktum/esp8266-vyvojova-deska-wemos-d1.html

### Important GPIO note

GPIO0 (D3) is a boot-strapping pin. The DS18B20 1-Wire bus normally has a pull-up, so the proposed connection is plausible, but **must be explicitly tested during reset/power-up before being accepted for production**.

GPIO16 (D0) has more limited interrupt functionality than the normal GPIOs. The PIR signal therefore uses polling rather than a timing-critical interrupt.

## 11. Test sequence enabled by Purchase No. 2

The purchase is intended to support the following sequence:

### Test 1 – Hall

Already underway. Complete quadrature and reset validation on Arduino Nano.

### Test 2 – ESP8266 Hall decoder

Move the validated Hall algorithm from Arduino to Wemos D1.

Acceptance:

- CW correct,
- CCW correct,
- rapid rotation correct,
- reset recovery correct,
- invalid transitions remain zero during normal operation.

### Test 3 – ESP + RF

Connect STX882 to the Wemos and reproduce the ControlRoll RF command.

Acceptance:

- roller responds correctly,
- no Hall counting loss during RF transmission,
- no Wi-Fi watchdog/reset problems.

### Test 4 – ESP + Hall + RF simultaneously

This is the key feasibility test for the one-ESP architecture.

Rotate the Hall test fixture while repeatedly transmitting RF commands.

Acceptance:

- no lost Hall transitions,
- no false direction changes,
- no unexpected resets,
- RF commands remain reliable.

### Test 5 – Sensors

Add:

- BH1750,
- DS18B20,
- PIR.

Verify simultaneous operation and MQTT/Home Assistant reporting.

### Test 6 – Integrated SmartRoll prototype

Combine:

```text
ESP8266
+ Hall A/B
+ RF TX
+ BH1750
+ DS18B20
+ PIR
+ Wi-Fi/MQTT
```

Only after Tests 2–5 pass should the integrated prototype be considered ready for mechanical installation.

## 12. Items intentionally NOT purchased yet

The following should remain OPEN until the above tests are complete:

- custom SmartRoll PCB,
- final enclosure,
- production mains PCB,
- final Hall sensor production quantity,
- final magnet production quantity,
- production connectors,
- final RF antenna type/length,
- final ESP8266 vs possible future ESP32 decision,
- any level shifter unless an actual sensor/RF interface requires one.

The project should avoid buying a large quantity of unverified components before the prototype architecture is validated.

## 13. Purchase priority

### MUST HAVE

1. WeMos D1 mini – 2 pcs
2. STX882 433 MHz transmitter – 2 pcs
3. 433 MHz antennas – 2 pcs
4. BH1750 – 1 pc
5. DS18B20 waterproof – 2 pcs
6. HC-SR501 – 2 pcs
7. jumper wires / headers / terminals
8. capacitors/resistors for prototype support

### RECOMMENDED

9. SRX882S receiver – 1 pc
10. 5 V / 3 A power supply – 1 pc
11. spare prototype PCB
12. heat-shrink and hookup wire

## 14. Procurement rule

Preferred suppliers for this project are:

- **Drátek.cz**
- **LaskaKit.cz**

Before ordering, availability and price should be checked again because they can change.

This list is the procurement baseline for the second purchasing round. Any substitution of a component with a different electrical interface shall be recorded in the SmartRoll documentation before it is used in a test.
