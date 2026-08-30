# SmartRoll – Test 6: Complete SmartRoll firmware

## Purpose

Test the first complete application firmware on one ESP32.

The firmware combines the subsystems already validated in Tests 2–5:

```text
ESP32
 ├── Hall A/B quadrature + direction
 ├── ERTE 433 MHz RF control
 ├── BH1750 light sensor
 ├── DS18B20 temperature sensor
 ├── PIR motion sensor (provisional GPIO)
 ├── Wi-Fi
 └── MQTT / Home Assistant
```

The objective is no longer to prove individual components. It is to prove that the complete SmartRoll application can run continuously and communicate with Home Assistant over the dedicated IoT network.

## Home Assistant communication

The selected integration method is **MQTT**.

This is deliberate because the project already uses Mosquitto and MQTT, and the SmartRoll firmware contains custom Arduino code for the proven ERTE RF protocol and Hall interrupt decoder. MQTT also works cleanly across routed VLANs as long as the firewall permits the ESP32 to reach the MQTT broker.

Home Assistant supports MQTT discovery, allowing the ESP32 to create its device/entities automatically. urlHome Assistant MQTT documentationhttps://www.home-assistant.io/integrations/mqtt

The firmware publishes MQTT discovery messages and exposes:

- roller-blind cover control (UP / DOWN / STOP),
- temperature,
- illuminance,
- PIR motion,
- Hall direction,
- relative Hall position/count,
- invalid Hall transitions,
- Wi-Fi RSSI,
- firmware version,
- online/offline availability.

## Network topology

```text
                         Home network
                              │
                     ┌────────┴────────┐
                     │ Synology NAS     │
                     │ Home Assistant   │
                     │ Mosquitto        │
                     └────────┬────────┘
                              │
                         routed network
                              │
                     ┌────────┴────────┐
                     │ MikroTik IoT AP  │
                     │ dedicated Wi-Fi │
                     └────────┬────────┘
                              │
                         Wi-Fi / IoT
                              │
                           ESP32
```

Only the MQTT broker needs to be reachable from the ESP32. The recommended firewall model is to allow **ESP32/IoT → MQTT broker TCP 1883** and keep other IoT-to-LAN access blocked unless separately required.

## Required configuration before flashing

Edit the configuration section at the top of `smartroll_test6.ino`:

```cpp
const char* WIFI_SSID = "YOUR_IOT_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* MQTT_HOST = "YOUR_MQTT_BROKER_IP";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_USER = "YOUR_MQTT_USER";
const char* MQTT_PASSWORD = "YOUR_MQTT_PASSWORD";
```

Do not commit real Wi-Fi or MQTT passwords to GitHub.

## Hardware pins

The pins remain based on the successfully tested configuration:

| Function | ESP32 GPIO | Status |
|---|---:|---|
| Hall A | GPIO32 | validated |
| Hall B | GPIO33 | validated |
| ERTE RF DATA | GPIO25 | validated |
| BH1750 SDA | GPIO21 | validated |
| BH1750 SCL | GPIO22 | validated |
| DS18B20 DATA | GPIO27 | validated |
| PIR OUT | GPIO26 | provisional for Test 6 |

The PIR pin is provisional because the production PIR sensor and final electrical interface have not yet been frozen in the specification.

## Parallel operation architecture

The ESP32 executes one firmware, not several independent programs. Functions are kept independent and serviced cooperatively:

```text
                    ESP32 firmware
                          │
       ┌──────────────────┼──────────────────┐
       │                  │                  │
   GPIO ISR            main loop          timers
       │                  │                  │
  Hall A/B          RF commands       sensors/MQTT
  immediate         Wi-Fi/MQTT        periodic jobs
  transitions       processing        status publish
```

The Hall ISR performs only the minimum transition decoding. It does not perform MQTT, Serial, sensor reads or RF transmission.

Sensor reads and MQTT processing are performed outside the ISR.

This architecture is essential because Hall edges must not be lost while the ESP32 is handling network traffic.

## Serial Monitor

Arduino IDE Serial Monitor remains available for local diagnostics at **115200 Bd**.

Commands:

```text
u = UP
s = STOP
d = DOWN
p = print complete status
r = reset relative Hall counters
h = help
```

The serial output is diagnostic only. Normal Home Assistant control is performed through MQTT.

## Home Assistant entities

After MQTT discovery succeeds, Home Assistant should discover one SmartRoll device containing:

- `cover.smartroll` – UP/DOWN/STOP
- temperature sensor
- illuminance sensor
- motion binary sensor
- direction sensor
- Hall position/count sensor
- Hall invalid-transition counter
- Wi-Fi RSSI
- firmware version

The cover intentionally does **not** expose an absolute percentage position yet. Absolute position calibration is still OPEN in the project specification.

## Test 6 sequence

### 6A – local complete firmware

- boot ESP32
- verify Wi-Fi connection
- verify MQTT connection
- verify BH1750
- verify DS18B20
- verify Hall state
- verify RF subsystem
- verify PIR input
- verify Serial diagnostics

### 6B – Home Assistant discovery

- verify SmartRoll device appears in MQTT integration
- verify all entities appear
- verify availability changes correctly

### 6C – Home Assistant RF control

- UP from HA
- STOP from HA
- DOWN from HA
- verify physical movement
- verify Hall direction/count simultaneously

### 6D – sensors + movement

While the blind is moving:

- Hall continues decoding
- temperature continues updating
- illuminance continues updating
- PIR continues detecting motion
- MQTT remains connected

### 6E – Wi-Fi/MQTT reconnect

- temporarily interrupt Wi-Fi
- restore Wi-Fi
- verify MQTT reconnect
- verify entities become available again
- verify Hall subsystem remains operational

### 6F – ESP32 reset

- reset ESP32 while stationary
- reset ESP32 during/after movement
- verify startup Hall state acquisition
- verify RF operation
- verify sensor operation
- verify MQTT/HA reconnection

### 6G – long combined operation

Operate the complete unit through repeated UP/STOP/DOWN cycles while HA and all sensors remain active.

## Acceptance criteria

Test 6 passes when:

1. ESP32 boots reliably.
2. Wi-Fi connects to the dedicated IoT network.
3. MQTT connects to the broker.
4. Home Assistant discovers the SmartRoll device.
5. UP/STOP/DOWN can be commanded from HA.
6. ERTE RF remains reliable.
7. Hall direction remains correct during HA/RF control.
8. Invalid Hall transitions remain zero during normal operation.
9. BH1750 and DS18B20 provide valid measurements.
10. PIR reports motion correctly.
11. Wi-Fi/MQTT reconnect does not break Hall/RF operation.
12. ESP32 reset restores complete operation.

## Important limitation

This is the first complete application firmware. It is **not yet the final production firmware**.

Still open after Test 6:

- exact Hall count → blind position calibration,
- production PIR selection and mounting,
- final power-supply validation,
- OTA update policy,
- long-duration reliability,
- final enclosure/mechanical integration.
