# SmartRoll – Test 5: ESP32 + Hall + RF + sensors

## Purpose

Add the two remaining local sensors to the already verified ESP32 Hall + ERTE RF subsystem:

- **BH1750** – ambient light level
- **DS18B20** – temperature

The aim is to verify that sensor reading does not disturb Hall quadrature decoding or ERTE RF control.

This test is intentionally still local. Wi-Fi, MQTT and Home Assistant are not included.

## Architecture

One ESP32 runs one firmware containing independent functions:

```text
ESP32
 ├── Hall A/B interrupt decoder
 ├── ERTE RF transmitter
 ├── BH1750 via I2C
 └── DS18B20 via 1-Wire
```

Hall transitions are handled by GPIO interrupts. Sensor reading and serial/RF handling remain in the main program. Sensor operations must not be placed inside the Hall ISR.

## Hardware

### Hall

Use the already approved Test 4 wiring:

```text
Hall A OUT -> GPIO32
Hall B OUT -> GPIO33
Hall A/B VCC -> 3.3 V
Hall A/B GND -> GND
10 kΩ pull-up from each OUT to 3.3 V
```

### ERTE RF

Use the already approved Test 4 wiring:

```text
RF DATA -> GPIO25
RF GND  -> ESP32 GND
RF VCC  -> module-rated supply
```

Do not change the proven RF timing or payload.

### BH1750

```text
BH1750 VCC -> ESP32 3.3 V
BH1750 GND -> ESP32 GND
BH1750 SDA -> ESP32 GPIO21
BH1750 SCL -> ESP32 GPIO22
```

The BH1750 is an I2C device. If the breakout already contains I2C pull-ups, no additional pull-ups should be added unless required by the particular board.

### DS18B20

```text
DS18B20 VDD  -> ESP32 3.3 V
DS18B20 GND  -> ESP32 GND
DS18B20 DATA -> ESP32 GPIO27
```

Add:

```text
4.7 kΩ between DATA and 3.3 V
```

Use the 3-wire powered configuration; do not use parasite power for this test.

## Required libraries

Arduino IDE:

- `BH1750`
- `OneWire`
- `DallasTemperature`
- `RF433send`

## Serial Monitor

Arduino IDE Serial Monitor:

```text
115200 Bd
```

Commands:

```text
u = UP
s = STOP
d = DOWN
p = print complete status
r = reset Hall counters / relative position
h = help
```

The firmware also prints sensor readings periodically:

```text
BH1750: 245.31 lx
DS18B20: 23.56 C
Hall: A=1 B=1 Position=... CW=... CCW=... Invalid=0
RF: idle
```

## Test sequence

### Test 5A – sensors alone

1. Start ESP32.
2. Confirm BH1750 gives a plausible lux value.
3. Confirm DS18B20 gives a plausible temperature.
4. Leave the system running for several minutes.
5. Confirm no resets or sensor communication errors.

### Test 5B – sensors + Hall

1. Start the firmware.
2. Observe sensor readings.
3. Rotate the Hall test fixture in both directions.
4. Verify CW/CCW decoding.
5. Verify `Invalid=0`.
6. Confirm sensor readings continue while Hall transitions occur.

### Test 5C – sensors + RF

1. Send UP.
2. Send STOP.
3. Send DOWN.
4. Confirm the roller responds correctly.
5. Confirm BH1750 and DS18B20 continue reporting.

### Test 5D – all functions simultaneously

Run several complete cycles:

```text
UP -> STOP -> DOWN -> STOP
```

while monitoring:

- Hall direction
- Hall transition count
- invalid transitions
- RF command result
- BH1750 reading
- DS18B20 reading

### Test 5E – reset

1. Run the complete system.
2. Reset ESP32.
3. Confirm both sensors initialize again.
4. Confirm Hall state initializes correctly.
5. Send UP/STOP/DOWN.
6. Confirm RF remains operational.

## Pass criteria

Test 5 passes when:

- BH1750 provides stable plausible readings.
- DS18B20 provides stable plausible readings.
- Hall CW/CCW remains correct.
- `Invalid=0` during normal movement.
- RF UP/STOP/DOWN remains reliable.
- Sensor reading does not cause missed Hall transitions.
- Sensor reading does not corrupt RF transmission.
- ESP32 reset restores all four subsystems.

## Important

This test validates sensor coexistence only. It does **not** yet validate Wi-Fi, MQTT or Home Assistant.

Those are added in Test 6.
