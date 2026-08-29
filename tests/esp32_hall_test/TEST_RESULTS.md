# SmartRoll – ESP32 Hall Test Results

## Test 2 / ESP32 Hall Test 1

**Evidence:** `evidence/esp32_test_hall1.txt`

The first ESP32 test consisted of the assembled Hall test fixture being powered and manually rotated five turns clockwise and five turns counter-clockwise, followed by a stop.

### Result

The test demonstrated that the ESP32 correctly reads Hall A/B transitions and the quadrature decoder identifies the rotation direction without false direction changes. No invalid transitions were observed.

The first test was considered directionally successful. A small difference between the counted forward and return transitions was observed, so a second controlled test was performed.

**Status: PASS – functional/directional verification**

---

## Test 2 / ESP32 Hall Test 2 – reset + 5 turns each direction

**Evidence:** `evidence/esp32_test_hall2.txt`

After an ESP32 restart, the fixture was rotated five complete turns in one direction, followed by a short pause, and then five complete turns back in the opposite direction.

### Measured result

- Initial Hall state: `11`
- First direction: `40` transitions, decoded as CCW
- Pause: state remained stable
- Return direction: `40` transitions, decoded as CW
- Total transitions: `80`
- Final position: `0`
- Invalid transitions: `0`
- No false direction changes observed
- After the test the state remained stable while stationary

The recorded log explicitly shows `CCW=40`, then `CW=40`, with `position=-40` after the first movement and `position=0` after the return. `invalid=0` throughout the recorded test.

**Status: PASS**

---

## Conclusion

The ESP32 implementation of the validated SmartRoll A/B quadrature decoder is accepted for the next development stage.

The test confirms:

1. Hall A/B can be read reliably by the ESP32.
2. Clockwise and counter-clockwise rotation are distinguished correctly.
3. The decoder survives an ESP32 restart and starts from the currently observed Hall state.
4. A complete movement in one direction followed by the same movement back returns the relative position to zero.
5. No invalid Hall state transitions were recorded in the controlled test.

### Important design decision

Absolute roller position after a power failure is **not required**. The motor's electronic upper and lower limits remain the authoritative absolute limits. SmartRoll only needs reliable relative movement/direction tracking; after a complete open or close cycle the software can resynchronize its relative position.

## Next stage

**Test 3 – ESP32 + ERTE RF**

The next test will isolate the RF subsystem. Hall sensors will not be involved initially. The objective is to verify that the already decoded and fixed ERTE RF protocol used by ControlRoll can be generated reliably by the ESP32 before combining RF control with Hall feedback.