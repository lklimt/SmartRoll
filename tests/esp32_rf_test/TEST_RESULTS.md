# SmartRoll – Test 3 Results

Status: **NOT TESTED YET**

## Test hardware

- ESP32-DevKit 38-pin: ____________________
- RF transmitter module: ____________________
- ERTE roller / receiver: ____________________
- Supply: ____________________

## Test A – basic commands

| Command | Function | Attempts | Successes | Failures | Result |
|---|---|---:|---:|---:|---|
| `01C` | UP | | | | |
| `01B` | STOP | | | | |
| `01A` | DOWN | | | | |

## Test B – 10 repetitions

- UP: ___ / 10
- STOP: ___ / 10
- DOWN: ___ / 10

## Test C – after ESP32 reset

- UP: ___
- STOP: ___
- DOWN: ___

## Test D – rapid sequence

Sequence:

```text
01C -> 01B -> 01A -> 01B
```

Result: ______________________________

## Evidence

Add the serial log and observations to:

`tests/esp32_rf_test/evidence/`

Suggested filename:

`esp32_test_rf1.txt`

## Final result

**PASS / FAIL / PARTIAL**

Notes:

__________________________________________________

__________________________________________________
