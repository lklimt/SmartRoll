# SR-HALL-TEST-002 — Arduino Nano + 2× A3144 Wiring

## Power

The complete test assembly is powered from the Arduino Nano USB connection.

**No external power supply is required.**

Use the Arduino Nano **5V** pin as the Hall sensor supply and a Nano **GND** pin as the common ground.

## Pin assignment

| Function | Arduino Nano | Hall A (A3144) | Hall B (A3144) |
|---|---|---|---|
| Supply | 5V | VCC | VCC |
| Ground | GND | GND | GND |
| Hall A output | D2 | OUT | — |
| Hall B output | D3 | — | OUT |

> **Important:** verify the actual pin numbering/pinout of the A3144 package received before applying power. Do not rely only on the physical orientation of a drawing.

## Hall A — exact connections

```text
Arduino Nano 5V
     |
     +---------------------> A3144 VCC
     |
    10 kΩ
     |
     +---------------------> A3144 OUT
     |
     +---------------------> Arduino D2

Arduino Nano GND ----------> A3144 GND
```

### Hall A bypass capacitor

Connect **100 nF ceramic** directly between the Hall A supply pins:

```text
A3144 VCC ----+---- 100 nF ----+---- A3144 GND
             |                 |
             +5 V              GND
```

Place this capacitor physically as close to the Hall sensor as practical.

## Hall B — exact connections

```text
Arduino Nano 5V
     |
     +---------------------> A3144 VCC
     |
    10 kΩ
     |
     +---------------------> A3144 OUT
     |
     +---------------------> Arduino D3

Arduino Nano GND ----------> A3144 GND
```

### Hall B bypass capacitor

Connect **100 nF ceramic** directly between the Hall B supply pins:

```text
A3144 VCC ----+---- 100 nF ----+---- A3144 GND
             |                 |
             +5 V              GND
```

Place this capacitor physically as close to the Hall sensor as practical.

## Complete connection overview

```text
                         ARDUINO NANO
                    +-------------------+
 USB 5 V ---------->| USB               |
                    |                   |
       +5 V --------| 5V                |
       GND ---------| GND               |
                    |                   |
       Hall A OUT --| D2                |
       Hall B OUT --| D3                |
                    +-------------------+
          |  |  |       |       |
          |  |  |       |       |
          |  |  +-------+-------+------------------- GND
          |  |          |       |
          |  |         10k     10k
          |  |          |       |
          |  +-------- Hall A  Hall B
          |             OUT     OUT
          |
          +----------------------------------------- +5 V

Hall A:
  VCC -> +5 V
  GND -> GND
  OUT -> D2
  10 kΩ from OUT to +5 V
  100 nF from VCC to GND

Hall B:
  VCC -> +5 V
  GND -> GND
  OUT -> D3
  10 kΩ from OUT to +5 V
  100 nF from VCC to GND
```

## Components

- Arduino Nano — 1×, powered by USB
- A3144 Hall sensor — 2×
- 10 kΩ resistor — 2×
- 100 nF ceramic capacitor — 2×
- USB cable — 1×
- test magnets
- jumper wires / breadboard as required

## Why the 10 kΩ resistors are here

The A3144 Hall switch has an open-collector output. The Arduino input therefore needs a pull-up to +5 V. Each Hall output receives its **own** 10 kΩ pull-up resistor.

Do not connect the 10 kΩ resistor from OUT to GND.

## Test inputs

- **D2 = Hall A**
- **D3 = Hall B**

These pins are intentionally chosen so the Arduino firmware can use interrupt-capable digital inputs for transition logging on a typical Nano/ATmega328P.

## Safety check before USB power

1. Confirm VCC-to-GND is not shorted on either Hall.
2. Confirm each 100 nF capacitor is between VCC and GND, not across OUT.
3. Confirm each 10 kΩ resistor is between +5 V and the corresponding OUT.
4. Confirm Hall A OUT goes only to D2.
5. Confirm Hall B OUT goes only to D3.
6. Confirm the exact A3144 package pinout from the received component documentation.
7. Only then connect USB power.
