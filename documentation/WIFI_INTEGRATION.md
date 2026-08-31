# SmartRoll – Wi-Fi integration requirements

## Purpose

SmartRoll uses the ESP32 Wi-Fi interface to communicate with the local MQTT broker (Mosquitto). During commissioning, the ESP32 must be correctly authenticated and authorized on the IoT Wi-Fi network and must receive the intended IP address.

## Required network configuration

When connecting a new SmartRoll ESP32 to the Wi-Fi network, verify **all** of the following:

1. **Wi-Fi authentication**
   - Correct IoT SSID is configured in the firmware.
   - Correct Wi-Fi password is configured locally in the firmware.
   - The ESP32 must be compatible with the authentication/security mode configured on the AP.

2. **MAC authorization / access control**
   - If the MikroTik/AP uses MAC-based authorization, access control, allow-lists or registration rules, the ESP32's Wi-Fi MAC address must be added/authorized.
   - Wi-Fi encryption alone does not guarantee that the device is permitted to join an AP when additional MAC authorization is enabled.

3. **DHCP authorization and address assignment**
   - The ESP32 must be permitted to obtain an address from the correct DHCP server/pool.
   - If the DHCP configuration uses static leases, address reservations, registration lists or another authorization mechanism, the ESP32 must be added there.
   - The assigned address must be from the intended IoT network/VLAN and must be reachable from the MQTT broker.

4. **MQTT connectivity**
   - After obtaining an IP address, verify that the ESP32 can reach the Mosquitto broker.
   - Verify MQTT credentials and port configuration.
   - MQTT should only be diagnosed after Wi-Fi and DHCP have been confirmed.

## Commissioning procedure

For each new ESP32 during initial commissioning:

### 1. Determine the ESP32 MAC address

Before adding the device to a restricted network, connect it temporarily to a suitable test network or run a diagnostic firmware that prints the Wi-Fi MAC address to the Serial Monitor.

Example:

```cpp
Serial.println(WiFi.macAddress());
```

Record the MAC address.

### 2. Authorize the device on the network

On the MikroTik/AP, if MAC authorization or registration is enabled, add the recorded MAC address to the appropriate authorization/allow-list.

### 3. Configure DHCP

Ensure that the ESP32 is allowed to use the correct DHCP pool. If a static DHCP lease/reservation is used, create it for the recorded ESP32 MAC address and assign the intended IP address.

### 4. Verify the connection

Use the ESP32 Serial Monitor and the MikroTik registration/DHCP tables to verify:

```text
ESP32 MAC
    ↓
Wi-Fi authentication
    ↓
MAC authorization (if enabled)
    ↓
DHCP authorization
    ↓
Correct IP address
    ↓
MQTT broker reachable
    ↓
MQTT connected
```

## Important lesson from Test 6A

The first complete Test 6A appeared to have a Wi-Fi/firmware problem. The actual cause was the MikroTik DHCP configuration: the ESP32 was not correctly registered in the DHCP pool and therefore did not obtain an IP address.

After correcting the DHCP configuration, the ESP32 connected normally and maintained Wi-Fi and MQTT communication.

Therefore, **DHCP configuration must be checked before changing the SmartRoll firmware when a newly commissioned ESP32 appears to have a Wi-Fi connection problem.**

## Diagnostic checklist

- [ ] Correct IoT SSID
- [ ] Correct Wi-Fi password
- [ ] ESP32 MAC address known and recorded
- [ ] MAC authorization checked (if enabled)
- [ ] ESP32 authorized on the AP/MikroTik
- [ ] Correct DHCP pool
- [ ] DHCP lease/reservation configured if required
- [ ] Correct IP address assigned
- [ ] Correct IoT VLAN/network
- [ ] MQTT broker reachable
- [ ] MQTT authentication verified

Real Wi-Fi passwords, MQTT passwords and other secrets must **never be committed to GitHub**.
