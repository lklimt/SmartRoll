/*
  SmartRoll - TEST 6B.2
  Home Assistant -> MQTT -> ESP32 -> ERTE RF

  IMPORTANT:
  This firmware intentionally preserves the validated Test 6A.3 implementation.
  No working Hall, sensor, Wi-Fi, DHCP or MQTT state-reporting logic is changed.

  6B.2 exercises the already validated MQTT command path for ERTE RF:
    UP   -> 01C
    STOP -> 01B
    DOWN -> 01A

  PIR is NOT part of SmartRoll.
  Serial Monitor: 115200 Bd
*/

// TEST 6B.2 firmware is intentionally the validated Test 6A.3 firmware.
// The RF MQTT command path was already present and is retained unchanged.
// Use the Test 6A.3 source as the authoritative implementation.

#include "../esp32_complete_test/smartroll_test6a_3.ino"
