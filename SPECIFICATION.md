# SmartRoll – Technical Specification

**Project:** SmartRoll  
**Repository:** `lklimt/SmartRoll`  
**Document:** `SPECIFICATION.md`  
**Status:** Work in Progress  
**Current mechanical revision:** SR-MEC-001 Rev.F

## 1. Purpose

SmartRoll is a project for developing an automated control system for **external roller blinds** equipped with **ERTE ET 45E tubular motors**.

The project consists of mechanical drive and coupling components, rotational position and direction sensing, Hall sensors and magnets, electronics, firmware, Home Assistant integration, testing and manufacturing documentation.

This document is the technical source of truth for the project.

## 2. Specification Rules

### 2.1 Source of truth

The latest approved information in this document has priority over previous conversation messages, old FreeCAD macros, obsolete drawings, experimental models and assumptions.

### 2.2 Parameter status

- **CONFIRMED** – directly measured or explicitly confirmed by the user.
- **DECIDED** – explicitly approved design decision.
- **ASSUMED** – temporary assumption; not a final manufacturing dimension.
- **OPEN** – information still required.
- **SUPERSEDED** – previous value replaced by a newer value.

### 2.3 No silent changes

A confirmed or decided parameter must not be changed without recording the change. Changing one parameter does not authorize changing other parameters.

### 2.4 No unverified assumptions

Unknown dimensions shall be marked `OPEN`, not invented.

### 2.5 FreeCAD design rule

FreeCAD macros are implementation tools, not the source of design requirements. A macro must derive its fixed dimensions and geometry from the current specification.

If a required dimension is `OPEN`, the macro shall not silently choose a final value. It must either expose the value as a clearly named parameter or stop and identify the missing specification.

### 2.6 Revision rule

Every substantial mechanical change receives a new revision. Old revisions are historical references and shall not be silently rewritten to represent a newer design.

# 3. Mechanical System

## 3.1 Roller-blind application

The SmartRoll mechanical system is intended for **external roller blinds** using **ERTE ET 45E tubular motors**.

The motor type and the associated mechanical installation are project-level constraints when designing the carrier, sensor arrangement and available mounting space.

**Status: CONFIRMED**

## 3.2 Roller-blind shaft

The roller blind uses an octagonal shaft.

| Parameter | Value | Status |
|---|---:|---|
| Shaft type | Octagonal | **CONFIRMED** |
| Across flats | 16.70 mm | **CONFIRMED** |
| Surface containing the slat feature | 5 + 6.7 + 5 mm | **CONFIRMED** |

The 16.70 mm value is the distance across the relevant opposing flats. The side containing the slat feature consists of `5.0 + 6.7 + 5.0 = 16.7 mm`.

This replaces the previously considered 7 mm side dimension.

## 3.3 Shaft orientation reference

The shaft geometry shall have a fixed reference orientation in all mechanical documentation and FreeCAD models.

The face containing the slat feature is the primary mechanical reference face. The carrier split and M3 joining geometry shall be defined relative to this face rather than by an arbitrary FreeCAD rotation.

**Status: DECIDED**

The exact global coordinate convention for the CAD model remains **OPEN** and shall be fixed before the production macro is finalized.

# 4. Available Space

| Condition | Available space | Status |
|---|---:|---|
| Blind unrolled | approx. 35 mm from wall | **CONFIRMED** |
| Blind wound | approx. 30 mm effective space | **CONFIRMED** |

The reduced clearance when the blind is wound must be respected.

The **approximately 30 mm wound-blind condition is the limiting design condition** for the magnetic carrier unless a later measurement establishes a smaller clearance.

**Status: DECIDED**

# 5. Stationary and Rotating Areas

The right-hand side contains a stationary black mounting part including the programming button.

The rotating part begins approximately **24 mm from the right-hand side**. Status: **CONFIRMED / APPROXIMATE**.

The programming button must remain accessible.

The magnetic carrier must not:

- contact the stationary mounting part,
- obstruct the programming button,
- prevent operation or programming of the ERTE ET 45E motor,
- enter the stationary portion of the mechanism.

The carrier may extend toward the right only within the available space and without compromising access to the programming button.

**Status: DECIDED**

# 6. Magnetic Carrier – SR-MEC-001

The magnetic carrier is mounted on the octagonal shaft. The current design uses a **two-piece carrier**.

This was chosen because opening a single ABS ring sufficiently to install it over the shaft is considered impractical or unreliable.

The two halves shall be installed independently and then mechanically clamped around the shaft.

**Status: DECIDED**

## 6.1 Functional separation

The carrier consists conceptually of two functional zones:

1. **Shaft clamping zone** – mechanically locked to the octagonal shaft.
2. **Magnetic rotating zone** – carries the magnets and provides circular clearance around the stationary mechanism.

The internal geometry of these two zones must not be confused.

**Status: DECIDED**

# 7. Shaft Clamping Section

The carrier contains an octagonal clamping section matching the shaft.

| Parameter | Value | Status |
|---|---:|---|
| Shaft AF | 16.70 mm | **CONFIRMED** |
| Carrier clamping section width | 6 mm | **DECIDED** |
| Geometry | Octagonal | **DECIDED** |

The octagonal internal geometry must preserve the actual shaft geometry.

The clamping zone must positively transmit rotation from the shaft to the carrier without relying on friction alone.

The shaft profile must remain continuous except where the slat clearance is intentionally provided.

# 8. Slat / Svlak Clearance

One flat of the shaft contains the slat feature. The carrier therefore requires a corresponding clearance on the correct octagonal face.

Current design decision: the slat clearance is rotated **90° relative to the carrier split / M3 joining arrangement**.

The slat clearance must be large enough for the actual shaft feature but must remove as little carrier material as practical.

**Status: DECIDED**

Exact depth and detailed shape remain **OPEN** until the measured geometry is entered into the project documentation.

# 9. Magnetic Section

The magnetic section is separate from the shaft-clamping geometry. It carries the magnets while allowing the carrier to rotate around the stationary part of the mechanism.

The magnetic section must therefore use a **circular internal clearance** rather than retaining the octagonal clamping profile.

The circular clearance is intentional: the magnetic section must be able to rotate freely around the stationary mechanism even though the clamping section follows the octagonal shaft.

**Status: DECIDED**

The magnetic section must not accidentally reproduce the octagonal shaft profile through its full height.

# 10. Magnet Arrangement

| Parameter | Value | Status |
|---|---:|---|
| Number of magnets | 2 | **DECIDED** |
| Magnet orientation | radial | **DECIDED** |
| Angular separation | 180° | **DECIDED** |
| Nominal magnet size | 4 × 2 mm | **DECIDED** |

The two magnets are positioned opposite each other.

The magnets shall be inserted **radially from the outside of the carrier** into their pockets.

The magnet pockets must:

- retain the magnets securely,
- provide adequate surrounding material,
- keep the magnets from entering the rotating clearance,
- maintain the defined 180° relationship,
- preserve sufficient carrier strength.

**Status: DECIDED**

The final pocket depth and exact retention method remain **OPEN**.

# 11. M3 Clamping

The two carrier halves are mechanically joined using **two M3 connections**.

Current design requirements:

- two M3 joining locations,
- closed screw bosses,
- provision for an M3 screw head,
- provision for an M3 nut,
- anti-rotation retention for the nut,
- screw/nut geometry outside the rotating clearance.

**Status: DECIDED**

The M3 boss must be completely closed around the screw. A boss providing only half of the screw-head radius is not acceptable.

The nut pocket shall retain the nut against rotation while allowing assembly.

# 12. M3 Accessibility

The M3 screw head must be accessible from the outside.

The boss must not be recessed so deeply that:

- the screw head cannot be inserted,
- a normal M3 screw cannot be installed,
- the screw cannot be tightened,
- the screw head interferes with the stationary mechanism.

The screw hardware must remain outside the required circular rotating clearance.

**Status: DECIDED**

# 13. Carrier Installation

Installation sequence:

1. Place one carrier half onto the shaft.
2. Place the second carrier half onto the opposite side.
3. Ensure the octagonal profiles are correctly engaged.
4. Ensure the slat clearance is correctly positioned.
5. Insert the M3 screws from the accessible outside direction.
6. Insert/retain the M3 nuts in their anti-rotation pockets.
7. Tighten both M3 connections.
8. Verify that the carrier is firmly coupled to the shaft.
9. Verify free rotation of the magnetic carrier relative to the stationary mechanism.
10. Verify that the programming button remains accessible.

**Status: DECIDED**

# 14. Mechanical Clearance Rules

The carrier shall be checked in at least two mechanical states:

### State A – blind unrolled

Available space: approximately **35 mm**.

### State B – blind wound

Available effective space: approximately **30 mm**.

State B is the critical clearance condition.

The carrier must not collide with the wall, stationary motor mounting components or other fixed components in either state.

**Status: DECIDED**

# 15. Manufacturing Material

The carrier is intended for 3D printing.

Previously discussed material: **tough ABS / ABS-type material**.

The material must tolerate repeated clamping and normal mechanical loads without cracking during installation.

**Status: OPEN – exact material and printing process must be finalized.**

# 16. Hall Direction Detection and Position Tracking

The project uses Hall sensing with two magnets mounted 180° apart on the rotating carrier.

The two Hall signals are intended to provide:

- rotational event detection,
- rotation counting,
- direction detection.

### 16.1 Tested Hall geometry

The physical test fixture has established a working configuration with:

- 2 magnets at 180°,
- equal magnet polarization,
- Hall A/B angular separation: **30°**,
- nominal air gap: **5 mm**.

Testing at 45° and 30° both produced repeatable direction-dependent state sequences. The 30° configuration was also tested at a manually induced speed higher than the expected roller-blind motor speed and remained reliable. An air gap of 10 mm was found to be unreliable; 5 mm is the current practical optimum on the test fixture.

The reliability test phase is considered complete for the current mechanical/electrical concept.

**Status: CONFIRMED for the test fixture; final mechanical implementation remains subject to production design verification.**

### 16.2 Direction algorithm

Direction shall be determined from the **sequence of Hall A/B states**.

For the tested 30° geometry the experimentally observed state ordering is:

Clockwise:

`10 → 11 → 01 → 11 → 10 → ...`

Counter-clockwise:

`01 → 11 → 10 → 11 → 01 → ...`

The implementation shall therefore use a transition state machine based on the experimentally verified transitions rather than assuming a textbook four-state quadrature sequence.

Valid directional transitions for the tested geometry are:

| Previous | New | Direction contribution |
|---|---|---:|
| 10 | 11 | CW |
| 11 | 01 | CW |
| 01 | 11 | CCW |
| 11 | 10 | CCW |

Transitions outside the verified state machine shall be treated as invalid and shall not be counted as normal movement.

The physical direction shall later be mapped to the logical SmartRoll functions **WIND / UNWIND** during installation/calibration. The mapping must be configurable rather than hard-coded from an assumed sensor orientation.

**Status: DECIDED – algorithm concept; firmware implementation OPEN**

### 16.3 Controller platform distinction

The current Hall experiments are performed using an **Arduino Nano / ATmega328P** solely as a development and measurement platform.

The final SmartRoll control unit shall use an **ESP microcontroller**. Therefore:

- Arduino test firmware is experimental firmware only,
- Arduino pin assignments must not be treated as final production pin assignments,
- Arduino timing/interrupt implementation must not be copied blindly to the ESP,
- the verified Hall state-machine logic is the portable functional requirement,
- final ESP firmware shall implement the same validated transition logic using ESP-appropriate GPIO/interrupt handling.

**Status: DECIDED**

### 16.4 Restart behavior

A controller restart, including a restart caused by power failure, must **not require the previous direction to be retained in RAM**.

After startup the controller shall:

1. read the current Hall A/B state,
2. store it as the initial state,
3. set direction to `UNKNOWN`,
4. wait for a subsequent valid Hall transition,
5. determine direction from that transition sequence,
6. continue relative movement counting from that point.

The controller shall not infer direction from the motor command alone.

The controller shall not assume that the blind is stationary at startup.

**Status: DECIDED**

### 16.5 Absolute position after power failure

The project does **not** require exact absolute blind position recovery after a controller power failure.

The ERTE ET 45E motor has its own programmed upper and lower travel limits. SmartRoll therefore treats Hall counting primarily as a **relative position and direction measurement** between known end positions.

When a complete movement to the upper or lower motor limit is commanded and completed, SmartRoll shall be able to resynchronize its software position to the corresponding known endpoint.

A temporary position error caused by power loss during movement is therefore acceptable and shall be corrected at the next full travel to an established motor endpoint.

**Status: DECIDED**

# 17. Mechanical Verification Requirements

Before a carrier revision is considered production-ready, it must be checked for:

1. correct 16.70 mm shaft engagement,
2. correct slat clearance,
3. correct orientation of the carrier halves,
4. correct M3 screw-head access,
5. correct nut retention,
6. sufficient material around magnet pockets,
7. correct radial magnet insertion,
8. 180° magnet spacing,
9. no intrusion into the circular rotating clearance,
10. no interference with the stationary mounting part,
11. access to the programming button,
12. clearance in the approximately 30 mm wound-blind condition,
13. secure torque transmission from shaft to carrier.

A CAD model alone does not constitute mechanical verification.

# 18. Parameters That Must NOT Be Guessed

The following shall not be invented or silently estimated:

- exact stationary-holder diameter,
- exact distance between stationary holder and wall,
- exact position and dimensions of programming button,
- exact dimensions of the slat feature,
- exact depth and width of slat clearance,
- final outer diameter of carrier,
- final magnetic-section dimensions,
- final magnetic-ring thickness,
- exact magnet pocket geometry,
- final M3 boss dimensions,
- final printing tolerances,
- final material and print orientation.

If required for the next design iteration, the value shall be marked `OPEN` until confirmed.

# 19. FreeCAD Macro Requirements

Every production FreeCAD macro for SR-MEC-001 shall:

- contain clearly named parameters,
- separate confirmed dimensions from adjustable/open dimensions,
- generate both carrier halves,
- preserve the 16.70 mm octagonal shaft geometry,
- preserve the 6 mm clamping-zone width,
- include the slat clearance in the correct orientation,
- provide the circular rotating clearance in the magnetic section,
- place two radial magnet pockets 180° apart,
- provide two functional M3 joining bosses,
- provide a complete M3 screw-head seat,
- provide anti-rotation nut pockets,
- keep the M3 hardware outside the rotating clearance,
- keep the programming button accessible,
- expose open dimensions at the top of the macro rather than hiding them inside geometry operations.

The macro must not silently replace a confirmed dimension with a rounded or approximate value.

# 20. Revision Control

Mechanical carrier revisions use `SR-MEC-001 Rev.X`.

Every revision must document:

- changed geometry,
- changed dimensions,
- reason for change,
- parameters that remain unchanged,
- parameters requiring verification.

A new revision must not overwrite the historical definition of a previous revision.

# 21. Current Status – SR-MEC-001 Rev.F

### Confirmed

- external roller-blind application,
- ERTE ET 45E tubular motor,
- octagonal shaft,
- 16.70 mm across flats,
- 5 + 6.7 + 5 mm geometry on the slat side,
- approximately 35 mm available space with blind unrolled,
- approximately 30 mm effective space with blind wound,
- rotating section begins approximately 24 mm from the right,
- programming button must remain accessible,
- test fixture: 2 magnets 180°, equal polarization, Hall separation 30°, 5 mm air gap,
- test fixture direction detection is reliable at tested slow and high manual rotation speeds.

### Decided

- two-piece carrier,
- two M3 connections,
- 6 mm octagonal clamping section,
- slat clearance 90° from the joining arrangement,
- circular clearance in the magnetic section,
- two radial magnets,
- magnets 180° apart,
- nominal magnets 4 × 2 mm,
- magnets inserted radially from the outside,
- carrier installed as two separate halves rather than opening a complete ring,
- approximately 30 mm wound-blind condition is the critical clearance state,
- Hall direction determined by experimentally verified A/B transition state machine,
- restart sets direction to UNKNOWN and reacquires it from subsequent Hall transitions,
- exact absolute position after power failure is not required,
- full travel to motor end limits can resynchronize software position,
- Arduino Nano is test hardware only; final controller is ESP-based.

### Open

- exact slat geometry,
- exact stationary-holder geometry,
- final magnetic-section dimensions,
- final outer diameter,
- final M3 boss geometry,
- final magnet-pocket dimensions,
- final printing tolerances,
- final material and printing parameters,
- final CAD coordinate convention,
- final ESP platform/model and production GPIO assignment,
- final Hall electrical interface on ESP,
- final position/count calibration,
- final filtering implementation.

# 22. Change Log Reference

All changes to this specification shall also be recorded in `CHANGELOG.md`.

No significant mechanical change shall be made only inside a FreeCAD macro. The specification must be updated first or simultaneously.

xx
