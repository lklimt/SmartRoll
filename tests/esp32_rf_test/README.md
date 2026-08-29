# SmartRoll – Test 3: ESP32 + ERTE RF

## Purpose

Verify that the ERTE FT45E RF protocol previously implemented and proven in ControlRoll can be generated directly by the ESP32.

This test intentionally isolates RF. Hall A/B, BH1750, DS18B20 and Wi-Fi/Home Assistant are not used.

The original ControlRoll implementation used the `RF433send` library with a TRIBIT encoder and the following timing parameters. The library officially supports ESP32, so Test 3 uses the same library and the same timing configuration rather than re-implementing the protocol. The Arduino Library listing identifies RF433send as supporting `avr`, `esp32` and `esp8266`. 

Reference:
- ControlRoll: `lklimt/ControlRoll`
- Original RF implementation: `arduino/ControlRoll/ControlRoll.ino`
- Original RF protocol documentation: `docs/communication.md`
- RF433send: https://github.com/sebmillet/rf433send

## Confirmed ERTE payloads

For roller 1:

```text
UP:   33 63 13 79 08
STOP: 33 63 13 79 2A
DOWN: 33 63 13 79 19
```

These are the same payloads used by ControlRoll and are therefore not being experimentally changed in Test 3.

## RF transmitter wiring

Use the same type of 433 MHz ASK/OOK transmitter used in the ControlRoll project.

Logical connection:

```text
RF transmitter VCC  -> module-rated supply voltage
RF transmitter GND  -> ESP32 GND
RF transmitter DATA -> ESP32 GPIO25
```

For a typical 5 V ASK transmitter module, power the module from its specified 5 V supply while keeping the DATA control signal at ESP32 3.3 V logic. Do not connect a 5 V signal back into an ESP32 GPIO.

**Important:** The exact transmitter module and its supply voltage must be checked against the physical module before powering it. If the module is not the same type as the proven ControlRoll transmitter, document the exact module in the evidence before declaring the test passed.

## Test sequence

### Phase 1 – basic transmission

1. Program the ESP32 with `esp32_erte_rf_test.ino`.
2. Connect only ESP32 and the RF transmitter.
3. Keep Hall sensors disconnected.
4. Open Serial Monitor at 115200 Bd.
5. Send `01C` – UP.
6. Send `01B` – STOP.
7. Send `01A` – DOWN.
8. Repeat each command several times.
9. Verify the controlled ERTE roller responds correctly.

### Phase 2 – repeated reliability

For each command perform at least 10 transmissions:

```text
10 × 01C (UP)
10 × 01B (STOP)
10 × 01A (DOWN)
```

Record every command and the physical response in an evidence text file.

### Phase 3 – restart

1. Reset ESP32.
2. Repeat UP, STOP and DOWN.
3. Confirm RF operation is unchanged after restart.

### Phase 4 – rapid command test

Send short sequences such as:

```text
01C
01B
01A
01B
```

with a reasonable pause between commands. Do not send overlapping RF transmissions.

## Acceptance criteria

Test 3 passes when:

- ESP32 generates the RF waveform using the same ERTE timing configuration as ControlRoll.
- UP reliably causes UP.
- DOWN reliably causes DOWN.
- STOP reliably causes STOP.
- No command is interpreted as another command.
- RF operation remains reliable after ESP32 reset.
- Repeated transmissions remain reliable.
- No Hall or other sensor functionality is required for RF operation.

## Important limitation

This test validates **RF generation from ESP32**, not yet the final combined SmartRoll firmware.

After Test 3 passes, the next integration stage is:

```text
ESP32
 ├── Hall A/B quadrature
 └── ERTE RF
```

Only after this combined test is stable should BH1750, DS18B20 and Wi-Fi/Home Assistant integration be added.

## Safety

The test must initially be performed with the RF transmitter connected to a low-voltage supply. The 230 V SmartRoll power supply is not part of Test 3.
