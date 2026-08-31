# SmartRoll – Test 6A – Result

**Status: PASS**

## Date
2026-08-31

## Result
The initial complete SmartRoll integration test was repeated after correcting the MikroTik DHCP configuration. The ESP32 DevKit had not previously been registered/allowed in the DHCP pool, so it could associate with Wi-Fi but could not obtain an IP address and therefore could not maintain the network connection.

After adding the ESP32 to the DHCP pool:

- Wi-Fi connection: **PASS**
- DHCP address assignment: **PASS**
- MQTT connection to Mosquitto: **PASS**
- MQTT communication verified with MQTT Explorer: **PASS**
- ERTE RF UP / DOWN / STOP: **PASS**
- Hall A/B quadrature decoding: **PASS**
- Hall direction detection: **PASS**
- BH1750 light measurement: **PASS**
- DS18B20 temperature measurement: **PASS**
- Parallel operation of the functions on one ESP32: **PASS**

## Evidence

Serial Monitor:
`evidence/esp32_complete_testA-oprava.txt`

MQTT Explorer:
`evidence/Mqtt-explorer.png`

The Serial Monitor evidence shows stable Wi-Fi and MQTT connections together with Hall counting/direction, RF commands, BH1750 readings and DS18B20 readings. The Hall decoder reports zero invalid transitions during the recorded test.

## Important finding

The earlier Test 6A Wi-Fi failure was **not a firmware or ESP32 Wi-Fi problem**. The cause was the MikroTik DHCP configuration: the ESP32 was not registered in the DHCP pool and therefore did not receive an IP address.

The Wi-Fi diagnostic test subsequently confirmed that the ESP32 could see and associate with the IoT WLAN. After correcting DHCP, the complete firmware operated normally.

## Conclusion

Test 6A is accepted as successful. The ESP32 platform is capable of running the Hall, ERTE RF, BH1750, DS18B20, Wi-Fi and MQTT functions concurrently.

The next development step is MQTT/Home Assistant integration (Test 6B), not further basic hardware validation.
