# SmartRoll – Test 6: Complete SmartRoll firmware

## Purpose

Test the first complete SmartRoll application firmware on one ESP32.

The firmware combines the subsystems already validated in Tests 2–5:

```text
ESP32
 ├── Hall A/B quadrature + direction
 ├── ERTE 433 MHz RF control
 ├── BH1750 light sensor
 ├── DS18B20 temperature sensor
 ├── Wi-Fi
 └── MQTT / Home Assistant
```

**PIR is not part of SmartRoll and is deliberately excluded.**

The first Test 6 build is intended for the laboratory fixture only. No live roller blind is required or affected.

## Home Assistant communication

The selected integration method is **MQTT**. The project already uses Mosquitto, while the ESP32 retains direct control of the proven ERTE RF protocol and Hall interrupt decoder.

Home Assistant MQTT discovery is used so the ESP32 can announce its device and entities automatically.

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

Recommended firewall policy:

```text
IoT ESP32 ───── TCP/1883 ─────> MQTT broker on NAS
```

Other IoT-to-LAN access should remain blocked unless separately required.

## Hardware pins

| Function | ESP32 GPIO | Status |
|---|---:|---|
| Hall A | GPIO32 | validated |
| Hall B | GPIO33 | validated |
| ERTE RF DATA | GPIO25 | validated |
| BH1750 SDA | GPIO21 | validated |
| BH1750 SCL | GPIO22 | validated |
| DS18B20 DATA | GPIO27 | validated |

## Parallel operation architecture

One ESP32 runs one firmware. The functions are independent software modules serviced cooperatively:

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

The Hall ISR performs only the minimum transition decoding. It does not perform MQTT, Serial, sensor reads or RF transmission. Sensor reads and MQTT processing run outside the ISR.

This is essential because Hall edges must not be lost while the ESP32 is handling network traffic.

## Serial Monitor

Arduino IDE Serial Monitor: **115200 Bd**.

Commands:

```text
u = UP
s = STOP
d = DOWN
p = print complete status
r = reset relative Hall counters
h = help
```

Serial remains the local diagnostic/control interface during Test 6. Normal Home Assistant control uses MQTT.

## MQTT topics

Device ID:

```text
smartroll_01
```

Base topic:

```text
smartroll/smartroll_01
```

Command:

```text
smartroll/smartroll_01/cover/command
```

Payloads:

```text
UP
DOWN
STOP
```

Availability:

```text
smartroll/smartroll_01/availability
```

State:

```text
smartroll/smartroll_01/state
```

The state JSON contains Hall state/counts, direction, illuminance, temperature and Wi-Fi RSSI.

## Home Assistant entities

The intended first HA entities are:

- SmartRoll cover – UP/DOWN/STOP
- temperature
- illuminance
- Hall direction
- relative Hall position/count
- invalid Hall transition counter
- Wi-Fi RSSI
- firmware version

Absolute percentage position is intentionally **not implemented yet**. Hall-count-to-blind-position calibration remains open.

## Test 6 sequence

### 6A – local complete firmware

- boot ESP32
- verify Wi-Fi connection
- verify MQTT connection
- verify BH1750
- verify DS18B20
- verify Hall state
- verify RF subsystem
- verify Serial diagnostics

### 6B – Home Assistant discovery

- verify SmartRoll device appears in MQTT integration
- verify entities appear
- verify availability changes correctly

### 6C – Home Assistant RF control

- UP from HA
- STOP from HA
- DOWN from HA
- verify RF command reception on the laboratory receiver/fixture
- confirm Hall direction/count simultaneously

### 6D – sensors + movement

While the laboratory fixture is moving:

- Hall continues decoding
- temperature continues updating
- illuminance continues updating
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

Operate the laboratory fixture through repeated UP/STOP/DOWN cycles while HA and all sensors remain active.

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
10. Wi-Fi/MQTT reconnect does not break Hall/RF operation.
11. ESP32 reset restores complete operation.

## Important limitation

This is the first complete application firmware. It is **not yet final production firmware**.

Still open after Test 6:

- exact Hall count → blind position calibration,
- final power-supply validation,
- OTA update policy,
- long-duration reliability,
- final enclosure/mechanical integration.
