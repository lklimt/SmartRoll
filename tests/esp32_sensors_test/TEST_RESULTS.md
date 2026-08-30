# SmartRoll – Test 5 Results

## ESP32 + Hall + ERTE RF + BH1750 + DS18B20

**Overall result: PASS**

All planned Test 5 stages A-E were completed successfully. The ESP32 operated the Hall decoder, ERTE RF transmitter, BH1750 and DS18B20 together without observed instability.

## Test 5A – sensors alone

Evidence: `evidence/esp32_all_sensors_testA.txt`

**PASS**

- BH1750 initialized successfully (`BH1750: OK`).
- Light readings were valid and stable during the test.
- DS18B20 produced valid temperature readings around 22.8 °C.
- Hall remained stationary with zero transitions and zero invalid transitions.
- RF reported ready.

## Test 5B – sensors + Hall

Evidence: `evidence/esp32_all_sensors_testB.txt`

**PASS**

Hall movement was decoded correctly in both directions while the sensors were running.

Observed:

- first direction: CCW increased to 48, CW remained 0
- return direction: CW increased to 56 while CCW remained 48
- final position returned to 0
- total transitions: 112
- invalid transitions: **0**

BH1750 and DS18B20 continued to provide valid readings throughout the Hall test.

## Test 5C – sensors + RF

Evidence: `evidence/esp32_all_sensors_testC.txt`

**PASS**

All three proven ERTE commands were transmitted while BH1750 and DS18B20 were active:

- `01A` DOWN – PASS
- `01B` STOP – PASS
- `01C` UP – PASS

Each RF command used 4 repetitions. Sensor readings remained valid and Hall remained valid with `invalid=0`.

## Test 5D – all functions simultaneously

Evidence: `evidence/esp32_all_sensors_testD.txt`

**PASS**

The complete combination of Hall + RF + BH1750 + DS18B20 was exercised during movement.

The log shows:

- RF DOWN followed by Hall CW movement
- RF STOP
- RF UP followed by Hall CCW movement
- sensor readings continuing throughout
- `invalid=0` throughout the recorded test

The recorded counters contain one isolated opposite-direction transition during the first movement (`CW=29, CCW=1`). This did not produce an invalid transition or destabilize the decoder and the subsequent movement was decoded correctly. The complete practical test was assessed as stable and functional.

## Test 5E – ESP32 reset

Evidence: `evidence/esp32_all_sensors_testE.txt`

**PASS**

After ESP32 reset:

- BH1750 initialized again successfully.
- DS18B20 produced valid readings around 22.5 °C.
- Hall initialized correctly.
- RF remained ready.
- DOWN, STOP and UP commands were transmitted again.
- Hall movement continued to be decoded with `invalid=0`.

## Overall conclusion

**TEST 5 – PASS ✓**

The ESP32 successfully operates all currently integrated SmartRoll functions concurrently:

```text
ESP32
 ├── Hall A/B quadrature decoder      ✓
 ├── ERTE 433 MHz RF                  ✓
 ├── BH1750 light sensor              ✓
 └── DS18B20 temperature sensor       ✓
```

The system remained stable during the tests and recovered correctly after ESP32 reset.

## Next stage

Proceed to **Test 6 – complete SmartRoll firmware integration**, adding the application logic, Wi-Fi/MQTT and Home Assistant communication while preserving the already verified Hall, RF and sensor subsystems.
