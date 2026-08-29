# SmartRoll – Test 4: ESP32 + Hall + ERTE RF

## Purpose

Test the two already verified subsystems running together on one ESP32:

```text
                 ┌── Hall A/B quadrature decoder
ESP32 ───────────┤
                 └── ERTE 433 MHz RF transmitter
```

The purpose is to verify that Hall movement tracking and RF control can operate concurrently without interfering with each other.

## Why several functions can run on one ESP32

The ESP32 is not running several programs simultaneously. It runs one firmware, but that firmware is divided into independent tasks that are serviced repeatedly by the processor.

For SmartRoll the important principle is:

- Hall A/B is handled by GPIO interrupts. A Hall edge can therefore be captured immediately even while the main program is doing other work.
- RF commands are generated only when requested and the RF transmission occupies the processor for a short, defined period.
- The main `loop()` remains non-blocking for normal status handling, command processing and diagnostics.
- The two subsystems share the same ESP32 but use different GPIOs and independent software state.

For the final SmartRoll firmware we will keep time-consuming operations out of the Hall interrupt handler. The ISR will only capture the Hall transition and update the minimal decoder state.

This test therefore establishes the architecture needed for the final unit before adding BH1750, DS18B20 and Wi-Fi/MQTT.

## Hardware

### Hall subsystem

```text
Hall A VCC  -> ESP32 3V3
Hall A GND  -> ESP32 GND
Hall A OUT  -> ESP32 GPIO32

Hall B VCC  -> ESP32 3V3
Hall B GND  -> ESP32 GND
Hall B OUT  -> ESP32 GPIO33

3V3 -> 10 kΩ -> Hall A OUT / GPIO32
3V3 -> 10 kΩ -> Hall B OUT / GPIO33
```

### RF subsystem

```text
433 MHz transmitter VCC  -> 5 V
433 MHz transmitter GND  -> ESP32 GND
433 MHz transmitter DATA -> ESP32 GPIO25
```

The transmitter is the tested three-pin VCC/DATA/GND module documented in Test 3.

## Serial Monitor interface

Arduino IDE Serial Monitor is the test control panel and diagnostic display.

Baud rate:

```text
115200
```

### Commands

```text
u = UP
s = STOP
 d = DOWN
r = reset software counters/relative position
p = print current status
h = print help
```

The space before `d` in this document is only typographical; the actual command is the single character `d`.

### Example

```text
> u
RF: UP sent

> p
Hall: A=1 B=1
Position: 0
CW: 0  CCW: 0  Invalid: 0
RF: idle

> d
RF: DOWN sent
```

While the blind is moving, Hall transitions are reported by the diagnostic output. The user can therefore see both activities in the same Serial Monitor session.

## What we are testing

The important point is that the user does **not** manually switch between separate Hall and RF programs. One firmware provides both functions through the Serial Monitor.

Example test:

```text
1. Send 'u' in Serial Monitor.
2. Blind starts moving UP.
3. Hall A/B transitions are captured automatically.
4. Position/direction counters change automatically.
5. Send 's'.
6. RF STOP is transmitted.
7. Hall activity stops when the blind stops.
8. Send 'd'.
9. Blind moves DOWN and Hall direction changes accordingly.
```

The Hall decoder must continue working regardless of whether an RF command was just sent.

## Test sequence

### A – stationary baseline

1. Start/reset ESP32.
2. Do not move the fixture/blind.
3. Observe Hall state for at least 30 seconds.
4. Verify no spontaneous transitions.

Expected:

```text
invalid = 0
position unchanged
```

### B – RF UP + Hall tracking

1. Send `u`.
2. Observe the blind moving UP.
3. Verify Hall transitions.
4. Verify decoded direction remains consistent.
5. Send `s`.
6. Verify the blind stops.

### C – RF DOWN + Hall tracking

1. Send `d`.
2. Verify movement DOWN.
3. Verify Hall direction changes to the opposite direction.
4. Send `s`.

### D – repeated cycles

Perform several:

```text
UP -> STOP -> DOWN -> STOP
```

Observe Hall counters after every cycle.

### E – RF commands while Hall is active

While the blind is moving and Hall transitions are being generated, issue STOP and then the opposite movement command. Verify that no Hall decoder corruption occurs.

### F – ESP32 reset

1. Move the blind.
2. Reset ESP32.
3. Verify boot and Hall initial state.
4. Send RF commands again.
5. Verify Hall decoding and RF operation remain functional.

## Pass criteria

Test 4 passes when:

- RF UP works while Hall decoding is active.
- RF STOP works while Hall decoding is active.
- RF DOWN works while Hall decoding is active.
- Hall CW/CCW direction remains correct during RF operation.
- `invalid=0` during normal test movement.
- No false direction changes are caused by RF transmission.
- Repeated UP/STOP/DOWN cycles remain stable.
- ESP32 reset does not prevent either subsystem from operating again.

## Important limitation

This test does not yet validate BH1750, DS18B20, Wi-Fi, MQTT or Home Assistant. Those are intentionally added later so that any failure remains attributable to the Hall/RF combination.
