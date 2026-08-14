# Hall Sensor Test Plan

## 1. Preparation

1. Obtain two identical candidate Hall sensors.
2. Obtain a small batch of identical candidate neodymium magnets.
3. Prepare the existing Arduino test board.
4. Print the adjustable test fixture from `freecad/SR-HALL-TEST-001.py`.
5. Assemble the fixture without permanently bonding sensors or magnets.

## 2. Electrical test

Connect Hall A and Hall B to separate Arduino digital inputs.

Record the exact sensor supply voltage, output logic polarity and pull-up arrangement.

First test each Hall individually with one magnet.

## 3. Distance test

For each sensor/magnet pair:

- start with a large air gap,
- slowly reduce the gap,
- record the first reliable activation point,
- continue closer,
- then move away and record the release point.

Repeat at least three times.

The final design gap must include a mechanical tolerance margin; it must not be set exactly at the switching threshold.

## 4. Pole/orientation test

Test both magnet faces and the intended radial orientation.

Record which orientation gives reliable switching.

The magnet pole orientation becomes a controlled assembly parameter if the candidate is approved.

## 5. A/B phase test

Install Hall A and Hall B in the adjustable angular fixture.

With the two-magnet carrier rotating slowly, record the complete A/B state sequence.

Adjust sensor angular spacing until a clean quadrature sequence is obtained.

The nominal 45° relationship is a starting hypothesis, not a confirmed final dimension.

## 6. Direction test

Rotate in both directions.

Confirm:

- one direction produces one valid sequence,
- the opposite direction produces the reverse sequence,
- no ambiguous transitions occur.

## 7. Reliability test

Repeat at least 20 complete revolutions in each direction.

Record any:

- missed pulse,
- extra pulse,
- invalid state transition,
- unstable switching,
- sensor dropout.

## 8. Final decision

Classify the candidate as:

- **APPROVED** – suitable for the SmartRoll design,
- **CONDITIONAL** – works but requires a defined mechanical/electrical limitation,
- **REJECTED** – unsuitable.

The result must be entered in the project BOM and Hall specification before the final carrier geometry is frozen.
