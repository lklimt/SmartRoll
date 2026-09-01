# SmartRoll – Test 6B.1 – Result

**Status: PASS**

## Result

The first MQTT → Home Assistant integration test was completed successfully.

The following values from the ESP32 SmartRoll firmware were correctly received and displayed by Home Assistant:

- DS18B20 temperature
- BH1750 illuminance
- Hall A
- Hall B
- Hall position
- Hall direction

The values update correctly when the physical laboratory test setup is operated or the measured conditions are changed.

## Communication path verified

```text
ESP32
  ↓ Wi-Fi
Mosquitto MQTT
  ↓
Home Assistant
  ↓
MQTT sensor entities
```

## Evidence

Evidence was stored in:

```text
 tests/esp32_complete_test/evidence/
```

with the Home Assistant screenshots using the `HA-...png` naming convention.

## Conclusion

Test 6B.1 is accepted as **PASS**.

The SmartRoll ESP32 is successfully integrated with Home Assistant through MQTT for sensor/state reporting.

No RF command was initiated from Home Assistant during this test.

## Next step

Proceed to **Test 6B.2 – Home Assistant → MQTT → ESP32 → ERTE RF**.
