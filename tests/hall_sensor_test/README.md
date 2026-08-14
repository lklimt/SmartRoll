# SmartRoll – Hall Sensor Test

## Purpose

This test is performed before finalizing the Hall sensor and magnet selection for the SmartRoll roller-blind position/direction system.

The test must verify the real magnetic/electrical behavior rather than relying only on catalog values.

## Test objectives

1. Verify that the candidate Hall sensor switches reliably with the candidate magnet.
2. Measure practical switching and release distance.
3. Verify that the magnet orientation is suitable for the intended radial carrier mounting.
4. Verify that two Hall sensors can produce a usable phase relationship.
5. Determine whether the intended two-magnet arrangement (180° apart) can produce reliable A/B quadrature transitions.
6. Determine a practical Hall-to-magnet air gap.
7. Identify whether the candidate Hall/magnet combination should be APPROVED or REJECTED.

## Current test status

**PLANNED – NOT YET TESTED**

## Important design rule

The current Hall sensor and magnet entries in the project BOM are candidates only. No component is approved for production until this test is completed.

## Test setup

- Arduino: existing test Arduino supplied by the project owner
- Hall sensors: 2 × candidate Hall sensor
- Magnets: candidate neodymium magnets, initially targeting approximately 4 × 2 mm
- Breadboard / prototype wiring
- 5 V supply suitable for the Arduino and Hall sensor
- Multimeter recommended
- 3D-printed adjustable test fixture
- PC with serial monitor/logger

## Test principle

A rotating test carrier or manually rotated magnet arrangement is used to reproduce the magnetic geometry expected in the final carrier.

The two Hall channels are called **A** and **B**.

The Arduino records both digital states and the sequence of transitions.

Expected quadrature behavior is:

Direction 1:

`00 → 01 → 11 → 10 → 00`

Direction 2:

`00 → 10 → 11 → 01 → 00`

The actual assignment of Direction 1/2 to WIND/UNWIND is intentionally left open until the final installation test.

## Test stages

### T1 – Single Hall / single magnet

- Verify wiring and logic level.
- Determine which magnet pole causes activation.
- Measure approximate switching distance.
- Measure release distance.
- Repeat at several air gaps.

### T2 – Radial magnet orientation

Mount the magnet in the same radial orientation intended for the final carrier.

Verify reliable switching while the magnet passes the Hall sensor tangentially.

### T3 – Two Hall sensors

Install Hall A and Hall B in an adjustable fixture.

Start with a candidate angular separation corresponding to the two-magnet geometry and adjust experimentally.

Record the Hall A/B waveform and transition order.

### T4 – Two magnets / 180° arrangement

Use two identical magnets positioned 180° apart on the test carrier.

Rotate the carrier slowly through at least two complete revolutions in each direction.

Record:

- Hall A state,
- Hall B state,
- transition sequence,
- number of transitions per revolution,
- missed transitions,
- false transitions.

### T5 – Speed test

Repeat the test at increasing rotation speeds until the expected maximum practical shaft speed is exceeded or the sensor system begins to miss transitions.

### T6 – Direction verification

Confirm that clockwise and counter-clockwise rotation produce opposite valid quadrature sequences.

## Acceptance criteria

The candidate system is acceptable only if:

- both Hall channels switch repeatably,
- no significant false switching occurs while stationary,
- the intended radial magnet orientation works,
- the A/B sequence is unambiguous in both directions,
- no transitions are systematically missed at the expected shaft speed,
- the selected air gap is mechanically achievable in the final carrier,
- the magnet does not interfere mechanically with the carrier or stationary parts.

## Measurements to record

| Parameter | Result | Status |
|---|---:|---|
| Hall sensor part number | TBD | OPEN |
| Magnet part number | TBD | OPEN |
| Magnet dimensions | TBD | OPEN |
| Magnet pole orientation | TBD | OPEN |
| Switching distance | TBD | OPEN |
| Release distance | TBD | OPEN |
| Practical air gap | TBD | OPEN |
| Hall A/B angular spacing | TBD | OPEN |
| Transitions / revolution | TBD | OPEN |
| Maximum tested speed | TBD | OPEN |
| False transitions | TBD | OPEN |
| Missed transitions | TBD | OPEN |
| Final result | TBD | OPEN |

## Evidence

Test photographs, oscilloscope/serial captures and measured values should be stored in this directory or its `evidence/` subdirectory.

Do not overwrite original measurement evidence.
