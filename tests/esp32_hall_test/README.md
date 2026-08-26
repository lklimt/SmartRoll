# SmartRoll – Test 2: ESP32 + Hall A/B

## Purpose

Verify that the quadrature Hall decoder already validated on the Arduino Nano works correctly on the ESP32.

This test deliberately contains **only ESP32 + Hall A + Hall B**. RF, Wi-Fi application communication, BH1750 and DS18B20 are not connected yet.

## Hardware

- ESP32-DevKitC 38-pin
- 2× Hall sensors from the approved Hall experiment
- 2× 10 kΩ pull-up resistors
- existing Merkur Hall test fixture
- jumper wires
- USB cable for ESP32 power/programming

## Wiring

### Hall A

| Hall A pin | Connect to |
|---|---|
| VCC | ESP32 3.3 V |
| GND | ESP32 GND |
| OUT | ESP32 GPIO32 |

A 10 kΩ resistor is connected between **Hall A OUT / GPIO32** and **3.3 V**.

### Hall B

| Hall B pin | Connect to |
|---|---|
| VCC | ESP32 3.3 V |
| GND | ESP32 GND |
| OUT | ESP32 GPIO33 |

A 10 kΩ resistor is connected between **Hall B OUT / GPIO33** and **3.3 V**.

## Complete wiring diagram

```text
                         ESP32
                  +------------------+
                  |                  |
        3V3 ------+----+-------------+----------------+
                  |    |                              |
                  |   10k                             10k
                  |    |                              |
                  |    +---- GPIO32                   +---- GPIO33
                  |         |                              |
                  |         |                              |
                  |      OUT Hall A                    OUT Hall B
                  |         |                              |
                  |      +--+--+                       +--+--+
                  |      | Hall |                       | Hall|
                  |      |  A   |                       |  B  |
                  |      +--+---+                       +--+--+
                  |         |                              |
        GND ------+---------+------------------------------+
                  |        GND                            GND
                  |
                  +------------------+
```

Equivalent point-to-point wiring:

```text
ESP32 3V3  -> Hall A VCC
ESP32 GND  -> Hall A GND
ESP32 GPIO32 -> Hall A OUT
3V3 -> 10 kΩ -> GPIO32 / Hall A OUT

ESP32 3V3  -> Hall B VCC
ESP32 GND  -> Hall B GND
ESP32 GPIO33 -> Hall B OUT
3V3 -> 10 kΩ -> GPIO33 / Hall B OUT
```

### Important

- ESP32 GPIOs are **3.3 V logic**.
- Do **not** connect a 5 V Hall output directly to GPIO32 or GPIO33.
- The exact Hall sensor output type must be confirmed. The 10 kΩ pull-ups are required when the selected Hall output is open-collector/open-drain.
- No RC capacitor is added in this first ESP32 test. We first verify the raw digital transitions.

## Mechanical arrangement

Use the already validated Hall test fixture:

- two magnets 180° apart,
- Hall A and Hall B directly beside each other,
- approximately 5 mm air gap.

The Hall sensors must remain mechanically fixed during the test.

## Expected quadrature sequences

Clockwise:

```text
11 -> 10 -> 00 -> 01 -> 11
```

Counter-clockwise:

```text
11 -> 01 -> 00 -> 10 -> 11
```

## Test procedure

1. Connect only ESP32 and Hall A/B.
2. Upload `esp32_hall_test.ino`.
3. Open Serial Monitor at **115200 baud**.
4. Leave the fixture stationary and verify that the state remains stable.
5. Rotate 5 complete turns clockwise.
6. Rotate 5 complete turns counter-clockwise.
7. Repeat at higher speed.
8. Press RESET on the ESP32 and repeat both directions.
9. Record the complete serial output in `evidence/`.

## Acceptance criteria

The test passes when:

- CW is always decoded as CW,
- CCW is always decoded as CCW,
- no false direction changes occur,
- `invalid = 0`,
- rapid rotation does not create invalid transitions,
- reset does not break decoding,
- the relative position returns to its starting value after equal clockwise and counter-clockwise movement.

Absolute roller position is **not** evaluated in this test.

## Next test

After Test 2 passes, the next step is **ESP32 + frozen ERTE RF protocol**, followed by the critical combined **Hall + RF + Wi-Fi concurrency test**.
