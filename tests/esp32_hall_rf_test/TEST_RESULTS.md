# SmartRoll – Test 4 Results

## ESP32 + Hall A/B + ERTE RF

All six Test 4 subtests A–F were completed successfully. The ESP32 remained stable and responded correctly throughout the combined Hall/RF testing.

## Test A – stationary baseline

Evidence: `evidence/esp32_hall_rf_testA.txt`

**PASS** – after startup the Hall state remained stable at `A=1 B=1`, position remained `0`, and `Invalid=0` throughout the stationary observation. The RF433send/TRIBIT configuration also loaded correctly. fileciteturn101file0L2-L2

## Test B – RF UP + Hall tracking

Evidence: `evidence/esp32_hall_rf_testB.txt`

**PASS** – the ESP32 transmitted `01C UP`; the log shows four RF repetitions and Hall transitions in the CW direction. Position increased from 0 to 36 with `CW=36`, `CCW=0`, `Invalid=0`. STOP was then transmitted successfully and the Hall count remained stable. fileciteturn102file0L2-L2

## Test C – RF DOWN + Hall tracking

Evidence: `evidence/esp32_hall_rf_testC.txt`

**PASS** – previously verified. DOWN produced consistent CCW Hall transitions with no invalid transitions.

## Test D – repeated UP/STOP/DOWN cycles

Evidence: `evidence/esp32_hall_rf_testD.txt`

**PASS** – repeated RF commands were accepted and the ESP32 remained stable. The test log shows repeated DOWN, STOP and UP commands, each with four RF repetitions, without reported Hall decoder errors. fileciteturn104file0L2-L2

## Test E – RF commands during Hall activity

Evidence: `evidence/esp32_hall_rf_testE.txt`

**PASS** – RF commands were issued while Hall activity was present. The Hall decoder continued to register movement in the expected direction; the observed transition counters remained consistent and `Invalid=0`. The test demonstrates that RF activity does not corrupt Hall decoding. fileciteturn105file0L2-L2

## Test F – ESP32 reset

Evidence: `evidence/esp32_hall_rf_testF.txt`

**PASS** – after ESP32 reset the firmware restarted, acquired the current Hall state and continued to decode movement correctly. Subsequent UP/DOWN operation produced consistent Hall counts with `Invalid=0`. fileciteturn106file0L2-L2

## Overall result

**PASS – Test 4 completed successfully.**

The combined ESP32 subsystem is considered stable for the tested configuration:

```text
ESP32
 ├── Hall A/B quadrature decoder  ✓
 └── ERTE 433 MHz RF control      ✓
```

Confirmed:

- Hall decoding and RF transmission coexist on one ESP32.
- RF UP/STOP/DOWN commands remain functional.
- Hall direction detection remains stable during RF operation.
- No invalid Hall transitions were observed in the recorded tests.
- Repeated RF movement cycles are stable.
- ESP32 reset does not prevent subsequent Hall/RF operation.

## Next stage

Test 4 is closed. The project can proceed to **Test 5 – adding BH1750 and DS18B20 while preserving the verified Hall + RF baseline**.

The Hall/RF firmware and the proven ERTE RF protocol must remain the reference baseline for the next integration step.
