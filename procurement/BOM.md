# SmartRoll – Bill of Materials (BOM)

## Purpose

Central purchasing list for the SmartRoll project.

**Preferred supplier:** Dratek.cz

The BOM separates confirmed project components from items that still depend on final electrical/mechanical verification. Product availability and prices are intentionally not frozen here; they must be checked at the time of purchase.

## 1. Mechanical carrier

| ID | Component | Specification | Qty | Status | Supplier |
|---|---|---|---:|---|---|
| MEC-001 | Printed magnetic carrier | SR-MEC-002 Rev.A Final Candidate; final material/print settings TBD | 1 set (2 halves) | DECIDED / CANDIDATE | 3D print |
| MEC-002 | Neodymium magnet | nominal 4 × 2 mm, 2 pcs, radial mounting, 180° apart | 2 | DECIDED; exact Dratek part OPEN | Dratek.cz |
| MEC-003 | Screw | M3, length to be confirmed from final carrier | 2 | OPEN | Dratek.cz |
| MEC-004 | Hex nut | M3, preferably suitable for anti-rotation pocket | 2 | DECIDED; exact type OPEN | Dratek.cz |

## 2. Hall sensing

| ID | Component | Specification | Qty | Status | Supplier |
|---|---|---|---:|---|---|
| SEN-001 | Hall sensor A | Digital Hall switch; exact part to be selected | 1 | OPEN | Dratek.cz |
| SEN-002 | Hall sensor B | Same type as Hall A | 1 | OPEN | Dratek.cz |
| SEN-003 | Sensor mounting hardware | According to final sensor holder design | TBD | OPEN | Dratek.cz / 3D print |

### Candidate Hall sensor at Dratek

Dratek currently lists **Hallův senzor 44E (A1344EUA-T)**, product code `1557994578`. It is a unipolar digital Hall sensor, specified for 4.5–24 V operation. This is a candidate only; it is **not yet approved as the final SmartRoll Hall sensor**, because the final magnetic polarity, sensor geometry and required quadrature behavior still need verification.

Reference: https://dratek.cz/arduino-platforma/7735-halluv-senzor-44e.html

## 3. Controller / electronics

| ID | Component | Specification | Qty | Status | Supplier |
|---|---|---|---:|---|---|
| ELE-001 | Microcontroller | Final controller architecture to be confirmed (Arduino/ESP platform) | 1 | OPEN | Dratek.cz |
| ELE-002 | Pull-up resistors | Value depends on Hall output and controller input | TBD | OPEN | Dratek.cz |
| ELE-003 | Decoupling capacitors | For Hall sensors and controller | TBD | OPEN | Dratek.cz |
| ELE-004 | Terminal/connectors | Final wiring design dependent | TBD | OPEN | Dratek.cz |
| ELE-005 | Power supply / DC-DC | Final input voltage and enclosure arrangement TBD | 1 | OPEN | Dratek.cz |

## 4. Wiring

| ID | Component | Specification | Qty | Status | Supplier |
|---|---|---|---:|---|---|
| CAB-001 | Sensor cable | Flexible multi-core cable, final length TBD | TBD | OPEN | Dratek.cz |
| CAB-002 | Wire/connectors | According to final controller installation | TBD | OPEN | Dratek.cz |

## 5. Current purchasing recommendation

Do **not** place the complete order yet.

The following items are reasonable candidates for early bench testing:

1. 2× Hall sensor 44E/A1344EUA-T.
2. M3 screws and M3 nuts for mechanical prototype testing.
3. Suitable small neodymium magnets for testing the Hall geometry.

The exact 4 × 2 mm magnet should be selected only after checking Dratek's current catalogue and the actual magnetic polarity required by the Hall sensor.

## 6. Important design rule

A component appearing in this BOM does not automatically mean that its exact commercial part number has been approved.

Status meanings:

- **CONFIRMED** – measured/verified project requirement.
- **DECIDED** – project design decision.
- **CANDIDATE** – possible component, not yet frozen.
- **OPEN** – selection still required.
- **SUPERSEDED** – historical item no longer valid.

No commercial component shall be frozen as the final part until its dimensions, electrical properties and mechanical compatibility with SmartRoll have been verified.

## 7. Supplier

Preferred supplier: **Dratek.cz**.

When purchasing, record the exact Dratek product name, product code, date of purchase and quantity in this BOM so that the build remains reproducible.
