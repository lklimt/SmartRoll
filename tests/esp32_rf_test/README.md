# SmartRoll – Test 3: ESP32 + ERTE RF

## Purpose

Verify that the ERTE FT45E RF protocol previously implemented and proven in ControlRoll can be generated directly by the ESP32.

This test intentionally isolates RF. Hall A/B, BH1750, DS18B20 and Wi-Fi/Home Assistant are not used.

The original ControlRoll implementation used the `RF433send` library with a TRIBIT encoder and the same timing configuration used here. The library supports ESP32, so Test 3 preserves the proven protocol instead of re-implementing it.

## Confirmed RF transmitter

The test uses the exact three-pin 433 MHz ASK/OOK transmitter module referenced by Components101:

- **VCC**
- **DATA**
- **GND**

Reference: https://components101.com/modules/433-mhz-rf-transmitter-module

### Logical wiring

```text
RF transmitter VCC  -> 5 V supply
RF transmitter GND  -> ESP32 GND
RF transmitter DATA -> ESP32 GPIO25
```

The transmitter is powered from its specified supply. The DATA control line is driven by the ESP32 at 3.3 V logic. No 5 V signal is connected back to an ESP32 GPIO.

For development the ESP32 and transmitter are powered from the low-voltage USB/5 V side. The 230 V SmartRoll power supply is not part of Test 3.

## Confirmed ERTE payloads

For roller 1:

```text
UP:   33 63 13 79 08
STOP: 33 63 13 79 2A
DOWN: 33 63 13 79 19
```

These are the same payloads used by ControlRoll and are frozen for this test.

## Test firmware

`esp32_erte_rf_test.ino`

Serial Monitor: **115200 Bd**

Commands:

```text
01C = UP
01B = STOP
01A = DOWN
```

## Test performed

The physical Test 3 setup was connected to an ERTE roller blind and tested directly from the ESP32.

Observed result:

- `01A` / DOWN: roller blind moved fully down.
- `01B` / STOP: roller blind stopped.
- `01C` / UP: roller blind moved fully up.

The blind accepted the ESP32-generated RF transmission and all three basic functions were demonstrated.

Evidence:

`evidence/ESP32-test-RF.txt`

## Current result

**PASS – basic ESP32 RF compatibility confirmed.**

This confirms that the ESP32 can directly generate the already proven ERTE RF protocol with the selected 433 MHz transmitter module.

## Remaining reliability checks

The basic functional test is passed. Before final SmartRoll integration, the following should still be recorded if desired:

- repeated UP/STOP/DOWN transmissions,
- operation after ESP32 reset,
- rapid command sequence.

These are reliability/robustness checks; they do not change the already proven RF payload or timing parameters.

## Next stage

After the RF basic test, integrate the two already verified subsystems:

```text
ESP32
 ├── Hall A/B quadrature
 └── ERTE RF
```

Only after this combined test is stable should BH1750, DS18B20 and Wi-Fi/Home Assistant integration be added.