# SmartRoll – Mechanical Design

## 1. Purpose

This document consolidates the physical measurements, approved design decisions and lessons learned from the FreeCAD development history of the SmartRoll magnetic carrier for external roller blinds with ERTE ET 45E motors.

The current design target is **SR-MEC-001**. Historical CAD revisions are evidence of development only; they are not automatically the current specification.

## 2. Physical reference geometry

### Shaft

- Octagonal roller-blind shaft.
- Confirmed across flats: **16.70 mm**.
- The face containing the slat feature is geometrically `5.0 + 6.7 + 5.0 mm`.
- The previously used **7.00 mm** side dimension is obsolete.

The repository also contains the note `Hridel - 8x 20 mm`. This should be interpreted as a nominal/profile description from the measurement note and must not replace the confirmed **16.70 mm across-flats** dimension.

### Installation envelope

- Cabinet depth: **160 mm**.
- Cabinet height: **160 mm**.
- Wound blind distance from wall: approximately **30 mm**.
- Available space when unwound: approximately **35 mm**.
- The wound condition is the limiting mechanical condition.
- The stationary black motor-holder and programming button are on the right side.
- The rotating section begins approximately **24 mm from the right-hand side**.

## 3. Approved carrier architecture

The carrier is divided into **two separate halves**.

This is an explicit design decision. The reason is installation: a complete ABS ring should not have to be elastically opened enough to pass over the shaft.

Installation is therefore:

1. place one half on the shaft;
2. place the second half on the opposite side;
3. engage the octagonal profiles;
4. align the slat clearance;
5. insert two M3 screws;
6. retain the M3 nuts in anti-rotation pockets;
7. tighten the carrier around the shaft.

## 4. Functional zones

The carrier has two different internal geometries.

### Zone A – shaft clamping

- Axial width: **6.00 mm**.
- Internal profile: octagonal.
- Reference shaft: **16.70 mm AF**.
- Function: positive torque transmission between shaft and carrier.

### Zone B – magnetic collar

- Axial width: **5.00 mm**.
- Total carrier axial width: **11.00 mm**.
- Internal profile: **circular**, not octagonal.
- Function: rotate freely around the stationary motor-holder.

This distinction is critical. Several historical CAD revisions explicitly document the correction from an octagonal magnetic bore to a circular bore.

## 5. Slat feature

The shaft has a slat feature on one octagonal face.

The carrier must provide clearance for this feature while retaining as much clamping material as possible.

The slat clearance is positioned **90° relative to the carrier joining arrangement**. This prevents the slat feature from being placed at the carrier split.

Exact clearance shape and depth remain OPEN until the physical feature is fully dimensioned.

## 6. Magnetic system

Two magnets are mounted in the rotating magnetic collar.

Approved properties:

- 2 magnets;
- 180° apart;
- radial orientation;
- insertion from the outside circumference;
- nominal magnet size: 4 × 2 mm.

The magnet pockets must not break through into the circular internal clearance. Final pocket dimensions remain dependent on the actual magnets and printing process.

## 7. M3 clamping system

Two M3 connections clamp the carrier halves together.

The design requirements established during the CAD iterations are:

- closed, mechanically complete M3 bosses;
- full support around the screw head;
- externally accessible screw head;
- anti-rotation hexagonal nut pocket;
- M3 hardware outside the circular rotating clearance;
- no interference with the stationary motor-holder.

A partially open boss or a boss containing only half of the screw-head radius is rejected.

## 8. Historical CAD development

The repository contains the following carrier development stages:

- `SR-MEC-001_RevA.FCMacro`
- `SR-MEC-001_RevB.FCMacro`
- `SR-MEC-001_RevC.FCMacro`
- `SR-MEC-001_RevD.FCMacro`
- `SR-MEC-001_RevE.FCStd`
- `SR-MEC-001_RevE1.FCMacro`
- `SR-MEC-001_RevE2.FCMacro`
- `SR-MEC-001_RevE3.FCMacro`
- `SR-MEC-001_RevE4.FCMacro`
- `SR-MEC-001_RevE5.FCMacro`

### Rev.A – initial complete concept

The first macro already contained several correct measured/design inputs:

- shaft AF 16.70 mm;
- 6 mm clamping length;
- 5 mm magnetic collar;
- 11 mm total width;
- two 4 × 2 mm magnets at 180°;
- M3 clamping concept.

However, it used a single split-ring concept and several provisional dimensions. It is historical only.

### Rev.E1

E1 explicitly established the important functional separation:

- 0–6 mm octagonal clamping bore;
- 6–11 mm circular Ø21 mm clearance;
- radial 4 × 2 mm magnets;
- magnets 180° apart;
- one continuous split.

The macro itself documents that the magnetic collar must not be octagonal because it has to rotate around the fixed holder.

### Rev.E2

E2 continued the single-slit architecture and explicitly intended the slit to allow the complete ring to be opened for shaft installation.

This concept was later rejected as impractical for tough ABS because sufficient elastic opening could not be guaranteed.

### Rev.E5

E5 is the latest macro currently present in the repository. It contains:

- 16.70 mm AF octagonal clamping section;
- 6 mm clamping width;
- 5 mm magnetic collar;
- Ø21 mm circular magnetic clearance;
- Ø26 mm outer diameter as a CAD reference;
- two closed M3 bosses;
- M3 head and nut pockets;
- radial magnet pockets at 90° / 270°;
- one continuous slot.

**Important:** E5 is the latest historical CAD implementation, but it is **not automatically the approved production revision** because the two-piece installation architecture and exact measured holder geometry still need to be reconciled.

## 9. Values that are CAD references, not physical measurements

The following values occur in the historical macros and must not be promoted to CONFIRMED without physical evidence:

- outer diameter Ø26 mm;
- magnetic clearance Ø21 mm;
- 0.20 mm shaft clearance;
- M3 hole Ø3.30 mm;
- M3 head recess values;
- nut pocket AF 5.50 mm;
- magnet pocket Ø4.20 mm × approximately 2.15 mm;
- slot width 0.80 mm.

They are retained as useful design-history references.

## 10. Design decisions superseding earlier concepts

### Superseded: single-piece ring opening

The carrier shall not depend on elastically opening a complete ABS ring for installation.

### Approved: two carrier halves

Both halves are placed separately around the shaft and clamped with two M3 connections.

### Superseded: octagonal magnetic bore

The magnetic collar must use circular clearance.

### Approved: radial magnets

Magnets are inserted from the outer circumference, not axially from the face.

### Superseded: 7 mm shaft side

The confirmed slat-side geometry is 5 + 6.7 + 5 mm.

## 11. Remaining critical measurements

Before a production FreeCAD macro is released, the following must be confirmed from the physical installation:

1. stationary-holder external diameter/profile;
2. exact right-side rotating/stationary boundary;
3. exact programming-button envelope;
4. exact slat feature depth and shape;
5. exact minimum wound-blind clearance at the carrier;
6. exact magnet dimensions of the selected parts;
7. exact Hall sensor mounting position;
8. final print-fit tolerances.

## 12. CAD rule for future revisions

Every future macro shall begin with a parameter block containing only:

- confirmed dimensions;
- explicit design decisions;
- clearly marked OPEN parameters.

A value copied from an older macro shall be labelled `REFERENCE` until independently confirmed.

No new FreeCAD macro is considered a production candidate merely because it renders successfully. It must first satisfy the mechanical specification and the installation/clearance checks.
