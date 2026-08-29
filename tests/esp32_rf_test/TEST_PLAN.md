# Test 3 – detailed test plan

## Objective

Prove that ESP32 can replace the Arduino Nano as the ERTE RF transmitter while preserving the already proven RF protocol.

## Test hardware

- ESP32-DevKit 38-pin
- 433 MHz ASK/OOK transmitter used by ControlRoll, or an explicitly documented equivalent
- ERTE FT45E roller blind / receiver
- USB cable
- low-voltage supply through USB during development

## Connections

```text
RF transmitter DATA -> ESP32 GPIO25
RF transmitter GND  -> ESP32 GND
RF transmitter VCC  -> transmitter's specified supply
```

No Hall sensor connection is required.

## Software

Arduino IDE + ESP32 board support.

Install library:

`RF433send` by Sébastien Millet.

The library is selected because it supports ESP32 and provides the same TRIBIT encoder used by the original ControlRoll implementation.

## RF configuration copied from ControlRoll

```text
Encoding:       TRIBIT
RF output:      GPIO25
Convention:     RFSEND_DEFAULT_CONVENTION
Parameter 4:    4
initseq:        9712 us
lo_prefix:      5048 us
hi_prefix:      1472 us
first_lo_ign:      0 us
lo_short:        362 us
lo_long:         722 us
hi_short:          0 us
hi_long:           0 us
lo_last:         704 us
separator:      9360 us
final parameter:  39
```

These values are frozen for Test 3. Do not tune them while evaluating ESP32 compatibility.

## Commands

Only roller 1 is tested initially:

| Command | Function | Payload |
|---|---|---|
| `01C` | UP | `33 63 13 79 08` |
| `01B` | STOP | `33 63 13 79 2A` |
| `01A` | DOWN | `33 63 13 79 19` |

## Test A – one command at a time

Perform:

1. `01C`
2. verify blind moves UP
3. `01B`
4. verify blind stops
5. `01A`
6. verify blind moves DOWN

Repeat the sequence at least 3 times.

## Test B – repeated commands

Perform 10 transmissions of each command.

Record:

- number of commands sent,
- physical response,
- any missed response,
- any unexpected response.

Target:

```text
UP:   10 / 10
STOP: 10 / 10
DOWN: 10 / 10
```

## Test C – ESP32 reset

1. Reset ESP32.
2. Wait for boot.
3. Send UP.
4. STOP.
5. DOWN.
6. Verify identical behaviour to Test A.

## Test D – rapid sequence

Send:

```text
01C
(wait)
01B
(wait)
01A
(wait)
01B
```

Use enough pause between commands for one RF transmission to finish. The purpose is to verify that repeated calls do not corrupt the next transmission.

## Pass criteria

Test 3 is PASS only if all of the following are true:

- UP works reliably.
- STOP works reliably.
- DOWN works reliably.
- No command causes an opposite movement.
- No unexpected movement occurs.
- Repeated transmissions remain reliable.
- Reset does not alter the result.
- The ESP32 implementation uses the same RF protocol/timing as ControlRoll.

## Failure handling

If the ESP32 transmission does not work while the Arduino Nano does:

1. Do not change the ERTE payload.
2. Do not immediately change timing parameters.
3. First verify transmitter wiring and supply.
4. Compare the actual RF output waveform with the Arduino version.
5. Only then investigate ESP32-specific timing behaviour.

This keeps the proven ControlRoll protocol as the reference implementation.
