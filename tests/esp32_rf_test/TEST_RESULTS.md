# SmartRoll – Test 3 Results

## Test hardware

- ESP32-DevKit 38-pin
- 433 MHz ASK/OOK transmitter, 3-pin VCC / DATA / GND, Components101 reference module
- ERTE roller blind / receiver
- USB / low-voltage development supply

## Test A – basic functional test

Evidence: `evidence/ESP32-test-RF.txt`

The first physical test was completed successfully. The ESP32 transmitted the proven ERTE RF protocol and the roller blind responded to all three basic commands.

| Command | Function | Result |
|---|---|---|
| `01C` | UP | PASS – blind moved UP |
| `01B` | STOP | PASS – blind stopped |
| `01A` | DOWN | PASS – blind moved DOWN |

The blind was fully controllable from the ESP32: DOWN, STOP and UP all worked.

## Result

**PASS – basic ESP32 RF compatibility confirmed.**

This is a functional compatibility result. The RF payload and timing configuration remain frozen and were not modified during the test.

## Remaining robustness tests

The following tests are still recommended before final integration:

### Test B – repeated commands

- UP: ___ / 10
- STOP: ___ / 10
- DOWN: ___ / 10

### Test C – after ESP32 reset

- UP: ___
- STOP: ___
- DOWN: ___

### Test D – rapid sequence

```text
01C -> 01B -> 01A -> 01B
```

Result: ______________________________

## Final status

**PARTIAL PASS – functional RF test passed; robustness testing remains.**

## Next stage

Once the basic RF robustness checks are complete, integrate the two verified subsystems:

```text
ESP32
 ├── Hall A/B quadrature
 └── ERTE RF
```

Then add BH1750, DS18B20 and Wi-Fi/Home Assistant integration.