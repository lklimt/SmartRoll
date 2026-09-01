# SmartRoll – Test 6B.2 – Result

**Status: PASS**

## Result

Home Assistant MQTT button entities successfully controlled the laboratory ERTE roller-blind receiver through the existing SmartRoll ESP32 firmware.

All three commands were tested:

- **DOWN** – PASS
- **STOP** – PASS
- **UP** – PASS

## Verified communication path

```text
Home Assistant
      ↓ MQTT
Mosquitto
      ↓
ESP32 SmartRoll
      ↓ RF 433 MHz
ERTE receiver
      ↓
Laboratory roller-blind test setup
```

## Important correction during test

The first HA buttons used the wrong command topic:

```text
smartroll/smartroll_01/command
```

The existing validated SmartRoll firmware 6A.3 actually subscribes to:

```text
smartroll/smartroll_01/cover/command
```

After correcting the HA command topic, all three commands operated correctly.

**Firmware 6A.3 was not modified.** The already validated RF, Hall, sensor, Wi-Fi and MQTT functionality was preserved unchanged.

## Conclusion

Test 6B.2 is accepted as **PASS**.

The complete command path from Home Assistant through MQTT to the ESP32 and finally to the ERTE RF receiver is verified.

## Next step

Proceed to the next integration stage: combine HA command control with Hall feedback so that Home Assistant can represent the actual roller-blind movement/state rather than only providing independent UP/STOP/DOWN buttons.
