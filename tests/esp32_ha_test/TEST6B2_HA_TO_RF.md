# SmartRoll – Test 6B.2
## Home Assistant → MQTT → ESP32 → ERTE RF

**Status:** READY FOR TEST

## Rule for this test

The already working SmartRoll firmware from Test 6A is **not to be rewritten or optimized**.

Test 6B.2 uses the already validated RF implementation, including:

- ERTE RF codes UP / STOP / DOWN
- `rfsend_builder(...)` configuration proven in previous tests
- existing Hall quadrature decoder
- existing BH1750
- existing DS18B20
- existing Wi-Fi/DHCP handling
- existing MQTT connection
- existing MQTT state reporting

Only the Home Assistant → MQTT command path is being exercised.

## Communication path

```text
Home Assistant
     │
     │ MQTT command
     ▼
  Mosquitto
     │
     ▼
   ESP32
     │
     │ RF433
     ▼
 ERTE receiver
```

## MQTT command topic

```text
smartroll/smartroll_01/command
```

Supported payloads:

```text
UP
STOP
DOWN
```

The existing validated firmware maps them to the already tested ERTE codes:

| MQTT payload | ERTE function | RF code |
|---|---|---|
| `UP` | Up | `01C` |
| `STOP` | Stop | `01B` |
| `DOWN` | Down | `01A` |

## Home Assistant test buttons

For the first test, use three temporary MQTT button entities. Add these to the existing `mqtt:` section of `configuration.yaml`:

```yaml
  - button:
      name: "SmartRoll Test UP"
      unique_id: smartroll_test_up
      command_topic: "smartroll/smartroll_01/command"
      payload_press: "UP"

  - button:
      name: "SmartRoll Test STOP"
      unique_id: smartroll_test_stop
      command_topic: "smartroll/smartroll_01/command"
      payload_press: "STOP"

  - button:
      name: "SmartRoll Test DOWN"
      unique_id: smartroll_test_down
      command_topic: "smartroll/smartroll_01/command"
      payload_press: "DOWN"
```

Do not create another top-level `mqtt:` section if one already exists.

## Test procedure

1. Start the laboratory SmartRoll ESP32.
2. Confirm Wi-Fi is connected and the ESP32 has its DHCP address.
3. Confirm MQTT is connected.
4. Confirm the existing sensor entities continue to update in HA.
5. Add the three temporary MQTT buttons.
6. Check HA configuration and restart HA if required by the installed configuration method.
7. Press **UP** once.
8. Confirm MQTT Explorer shows `UP` on the command topic.
9. Confirm the ESP32 Serial Monitor reports the received command and RF transmission.
10. Confirm the laboratory ERTE receiver responds.
11. Repeat for **STOP** and **DOWN**.
12. Repeat a short sequence such as `UP → STOP → DOWN → STOP`.
13. Verify that Hall, temperature and light reporting continue normally during RF operation.

## Expected Serial Monitor indication

For example:

```text
MQTT RX ...: UP
TX 01C UP : 33 63 13 79 08
RF send repetitions: ...
```

The exact repetition count is determined by the validated RF library configuration.

## PASS criteria

- [ ] HA UP button publishes `UP`.
- [ ] ESP32 receives `UP`.
- [ ] ERTE responds to UP.
- [ ] HA STOP button publishes `STOP`.
- [ ] ESP32 receives `STOP`.
- [ ] ERTE responds to STOP.
- [ ] HA DOWN button publishes `DOWN`.
- [ ] ESP32 receives `DOWN`.
- [ ] ERTE responds to DOWN.
- [ ] Short command sequence works.
- [ ] Wi-Fi remains connected.
- [ ] MQTT remains connected.
- [ ] Hall reporting remains functional.
- [ ] Temperature reporting remains functional.
- [ ] Light reporting remains functional.
- [ ] No unexpected RF command is generated without an MQTT command or local test command.

## Safety

This test is performed on the laboratory mechanical/electrical setup only. No live roller blind is controlled.

## Evidence

Store evidence under:

```text
 tests/esp32_ha_test/evidence/
```

Recommended:

```text
esp32_ha_testB2_serial.txt
mqtt_explorer_B2.png
home_assistant_B2.png
```

## Next step

After successful completion, continue with Test 6B.3 – combined bidirectional operation and state consistency.
