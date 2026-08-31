# SmartRoll – Test 6B.1
## MQTT → Home Assistant

**Status:** READY FOR TEST

### Purpose
Verify that the already validated SmartRoll ESP32 firmware can be integrated into Home Assistant through the existing Mosquitto MQTT broker.

Test 6B.1 is intentionally one-way: **ESP32 → MQTT → Home Assistant**.

RF control from HA is not tested yet. It belongs to Test 6B.2.

### Existing architecture

```text
ESP32 SmartRoll
      │
      │ Wi-Fi
      ▼
MikroTik IoT network
      │
      ▼
Mosquitto MQTT
      │
      ▼
Home Assistant
(Synology Docker)
```

### MQTT topics used by Test 6A/6B.1

Device ID used by the test firmware:

```text
smartroll_01
```

Base topic:

```text
smartroll/smartroll_01
```

State topic:

```text
smartroll/smartroll_01/state
```

Availability topic:

```text
smartroll/smartroll_01/availability
```

The state payload is JSON and contains:

```json
{
  "hall_a": 0,
  "hall_b": 0,
  "position": 0,
  "cw": 0,
  "ccw": 0,
  "transitions": 0,
  "invalid": 0,
  "direction": "UNKNOWN",
  "lux": 0.0,
  "temperature": 0.0,
  "rssi": -60
}
```

Values shown above are examples of the payload structure, not fixed sensor values.

### Test procedure

1. Verify that SmartRoll ESP32 is connected to the IoT Wi-Fi.
2. Verify that it has a valid DHCP address.
3. Verify MQTT connection to Mosquitto.
4. Verify in MQTT Explorer that the state and availability topics are present.
5. In Home Assistant, configure the MQTT integration if it is not already configured.
6. Create temporary MQTT entities using the topics below.
7. Observe the values in HA while operating the laboratory Hall mechanism and changing light/temperature conditions.
8. Confirm that values update without restarting HA.
9. Restart the ESP32 and confirm that availability returns to `online` after reconnection.

### Temporary Home Assistant MQTT entities

For Test 6B.1 use temporary YAML entities rather than MQTT Discovery. This keeps the test independent of the final HA device model.

Example:

```yaml
mqtt:
  sensor:
    - name: "SmartRoll Test Position"
      unique_id: smartroll_test_position
      state_topic: "smartroll/smartroll_01/state"
      value_template: "{{ value_json.position }}"
      availability_topic: "smartroll/smartroll_01/availability"
      payload_available: "online"
      payload_not_available: "offline"

    - name: "SmartRoll Test Direction"
      unique_id: smartroll_test_direction
      state_topic: "smartroll/smartroll_01/state"
      value_template: "{{ value_json.direction }}"
      availability_topic: "smartroll/smartroll_01/availability"
      payload_available: "online"
      payload_not_available: "offline"

    - name: "SmartRoll Test Temperature"
      unique_id: smartroll_test_temperature
      state_topic: "smartroll/smartroll_01/state"
      value_template: "{{ value_json.temperature }}"
      unit_of_measurement: "°C"
      device_class: temperature
      availability_topic: "smartroll/smartroll_01/availability"
      payload_available: "online"
      payload_not_available: "offline"

    - name: "SmartRoll Test Light"
      unique_id: smartroll_test_light
      state_topic: "smartroll/smartroll_01/state"
      value_template: "{{ value_json.lux }}"
      unit_of_measurement: "lx"
      device_class: illuminance
      availability_topic: "smartroll/smartroll_01/availability"
      payload_available: "online"
      payload_not_available: "offline"

    - name: "SmartRoll Test Hall A"
      unique_id: smartroll_test_hall_a
      state_topic: "smartroll/smartroll_01/state"
      value_template: "{{ value_json.hall_a }}"
      availability_topic: "smartroll/smartroll_01/availability"
      payload_available: "online"
      payload_not_available: "offline"

    - name: "SmartRoll Test Hall B"
      unique_id: smartroll_test_hall_b
      state_topic: "smartroll/smartroll_01/state"
      value_template: "{{ value_json.hall_b }}"
      availability_topic: "smartroll/smartroll_01/availability"
      payload_available: "online"
      payload_not_available: "offline"

    - name: "SmartRoll Test WiFi RSSI"
      unique_id: smartroll_test_wifi_rssi
      state_topic: "smartroll/smartroll_01/state"
      value_template: "{{ value_json.rssi }}"
      unit_of_measurement: "dBm"
      availability_topic: "smartroll/smartroll_01/availability"
      payload_available: "online"
      payload_not_available: "offline"
```

If the existing Home Assistant configuration uses another supported MQTT YAML structure/version, adapt the entity declaration to the installed HA version rather than changing the SmartRoll MQTT payload during this test.

### PASS criteria

Test 6B.1 is PASS when:

- [ ] ESP32 remains connected to Wi-Fi.
- [ ] ESP32 has a valid DHCP address.
- [ ] MQTT remains connected to Mosquitto.
- [ ] HA receives the SmartRoll availability state.
- [ ] Hall A updates in HA.
- [ ] Hall B updates in HA.
- [ ] Position updates in HA.
- [ ] Direction updates in HA.
- [ ] Temperature updates in HA.
- [ ] Illuminance updates in HA.
- [ ] RSSI updates in HA.
- [ ] ESP32 reconnects after a Wi-Fi interruption/restart.
- [ ] No RF command is generated by HA in this test.

### Evidence

Store evidence under:

```text
 tests/esp32_ha_test/evidence/
```

Recommended files:

```text
esp32_ha_testB1_serial.txt
mqtt_explorer_B1.png
home_assistant_B1.png
```

### Security

Wi-Fi and MQTT passwords must never be committed to GitHub. Use local configuration only.

### Next test

After Test 6B.1 passes, continue with **Test 6B.2 – Home Assistant → MQTT → ESP32 → ERTE RF**.
