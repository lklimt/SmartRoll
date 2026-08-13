# SmartRoll – Technical Specification

**Project:** SmartRoll  
**Repository:** `lklimt/SmartRoll`  
**Document:** `SPECIFICATION.md`  
**Status:** Work in Progress  
**Current mechanical revision:** SR-MEC-001 Rev.F

---

## 1. Purpose

SmartRoll is a project for developing an automated roller-blind control system.

The project consists of:

- mechanical drive and coupling components,
- rotational position and direction sensing,
- Hall sensors and magnets,
- electronics,
- firmware,
- Home Assistant integration,
- testing and manufacturing documentation.

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

# 3. Mechanical System

## 3.1 Roller-blind shaft

The roller blind uses an octagonal shaft.

| Parameter | Value | Status |
|---|---:|---|
| Shaft type | Octagonal | **CONFIRMED** |
| Across flats | 16.70 mm | **CONFIRMED** |
| Surface containing the slat feature | 5 + 6.7 + 5 mm | **CONFIRMED** |

The 16.70 mm value is the distance across the relevant opposing flats. The side containing the slat feature consists of `5.0 + 6.7 + 5.0 = 16.7 mm`.

This replaces the previously considered 7 mm side dimension.

# 4. Available Space

| Condition | Available space | Status |
|---|---:|---|
| Blind unrolled | approx. 35 mm from wall | **CONFIRMED** |
| Blind wound | approx. 30 mm effective space | **CONFIRMED** |

The reduced clearance when the blind is wound must be respected.

# 5. Stationary and Rotating Areas

The right-hand side contains a stationary black mounting part including the programming button.

The rotating part begins approximately **24 mm from the right-hand side**. Status: **CONFIRMED / APPROXIMATE**.

The programming button must remain accessible. The carrier may extend toward the right only within the available space and must not obstruct the button.

# 6. Magnetic Carrier – SR-MEC-001

The magnetic carrier is mounted on the octagonal shaft. The current design uses a **two-piece carrier**.

This was chosen because opening a single ABS ring sufficiently to install it over the shaft is considered impractical or unreliable.

**Status: DECIDED**

# 7. Shaft Clamping Section

The carrier contains an octagonal clamping section matching the shaft.

| Parameter | Value | Status |
|---|---:|---|
| Shaft AF | 16.70 mm | **CONFIRMED** |
| Carrier clamping section | 6 mm | **DECIDED** |
| Geometry | Octagonal | **DECIDED** |

The octagonal internal geometry must preserve the actual shaft geometry.

# 8. Slat / Svlak Clearance

One flat of the shaft contains the slat feature. The carrier therefore requires a corresponding clearance on the correct octagonal face.

Current design decision: the slat clearance is rotated **90° relative to the carrier split / M3 joining arrangement**.

**Status: DECIDED**

Exact depth and detailed shape remain **OPEN** until the measured geometry is entered into the project documentation.

# 9. Magnetic Section

The magnetic section is separate from the shaft-clamping geometry. It carries the magnets while allowing the carrier to rotate around the stationary part of the mechanism.

The magnetic section must therefore use a circular internal clearance rather than retaining the octagonal clamping profile.

**Status: DECIDED**

# 10. Magnet Arrangement

| Parameter | Value | Status |
|---|---:|---|
| Number of magnets | 2 | **DECIDED** |
| Magnet orientation | radial | **DECIDED** |
| Angular separation | 180° | **DECIDED** |
| Nominal magnet size | 4 × 2 mm | **DECIDED** |

The two magnets are positioned opposite each other. Magnet pockets must retain sufficient surrounding material and must not intrude into the rotating clearance.

# 11. M3 Clamping

The two carrier halves are mechanically joined using M3 screws.

Current design:

- two M3 joining locations,
- closed screw bosses,
- provision for an M3 screw head,
- provision for an M3 nut,
- screw/nut geometry outside the rotating clearance.

**Status: DECIDED**

The M3 boss must be completely closed around the screw. The screw head must be insertable from the outside. The nut must have an anti-rotation hexagonal pocket.

# 12. M3 Accessibility

The M3 screw head must be accessible from the outside. The boss must not be recessed so deeply that the screw head cannot be inserted or tightened, and it must not interfere with the stationary mechanism or rotating clearance.

**Status: DECIDED**

# 13. Carrier Installation

Installation sequence:

1. Place one carrier half onto the shaft.
2. Place the second carrier half onto the opposite side.
3. Ensure the octagonal profiles are correctly engaged.
4. Ensure the slat clearance is correctly positioned.
5. Insert the M3 screws.
6. Insert/retain the M3 nuts.
7. Tighten the two M3 connections.
8. Verify free rotation of the carrier relative to the stationary mechanism.

**Status: DECIDED**

# 14. Manufacturing Material

The carrier is intended for 3D printing. Previously discussed material: tough ABS / ABS-type material.

**Status: OPEN – exact material and printing process must be finalized.**

# 15. Hall Direction Detection

The project uses Hall sensing with two magnets mounted 180° apart on the rotating carrier. The two Hall transitions will be used for rotational counting and direction detection.

The exact Hall sensor arrangement, signal processing and firmware state machine are to be documented separately.

**Status: DECIDED – detailed implementation OPEN**

# 16. Parameters That Must NOT Be Guessed

The following shall not be invented or silently estimated:

- exact stationary-holder diameter,
- exact distance between stationary holder and wall,
- exact position of programming button,
- exact dimensions of the slat feature,
- exact depth of slat clearance,
- final outer diameter of carrier,
- final magnetic-ring thickness,
- exact magnet pocket geometry,
- final M3 boss dimensions,
- final printing tolerances.

If required for the next design iteration, the value shall be marked `OPEN` until confirmed.

# 17. Revision Control

Mechanical carrier revisions use `SR-MEC-001 Rev.X`.

Every revision must document changed geometry, changed dimensions, reason for change, parameters remaining unchanged, and parameters requiring verification.

A new revision must not overwrite the historical definition of a previous revision.

# 18. Current Status – SR-MEC-001 Rev.F

### Confirmed

- octagonal shaft,
- 16.70 mm across flats,
- 5 + 6.7 + 5 mm geometry on the slat side,
- approximately 35 mm available space with blind unrolled,
- approximately 30 mm effective space with blind wound,
- rotating section begins approximately 24 mm from the right,
- programming button must remain accessible.

### Decided

- two-piece carrier,
- two M3 connections,
- 6 mm octagonal clamping section,
- slat clearance 90° from the joining arrangement,
- circular clearance in the magnetic section,
- two radial magnets,
- magnets 180° apart,
- nominal magnets 4 × 2 mm.

### Open

- exact slat geometry,
- exact stationary-holder geometry,
- final magnetic-section dimensions,
- final outer diameter,
- final M3 boss geometry,
- final printing tolerances,
- final material and printing parameters.

# 19. Change Log Reference

All changes to this specification shall also be recorded in `CHANGELOG.md`.

No significant mechanical change shall be made only inside a FreeCAD macro. The specification must be updated first or simultaneously.
