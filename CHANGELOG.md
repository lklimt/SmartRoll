# SmartRoll Change Log

## 2026-08-13 – Initial documentation baseline

### Added

- Initial `SPECIFICATION.md`.
- Project documentation structure defined in `README.md`.
- Established parameter status system: `CONFIRMED`, `DECIDED`, `ASSUMED`, `OPEN`, `SUPERSEDED`.
- Established rule that confirmed/decided parameters must not be changed silently.
- Established SR-MEC-001 mechanical revision tracking.

### SR-MEC-001 Rev.F baseline

Confirmed:

- external roller-blind application,
- ERTE ET 45E tubular motor,
- octagonal shaft,
- 16.70 mm across flats,
- 5 + 6.7 + 5 mm geometry on the slat side,
- approximately 35 mm available space with blind unrolled,
- approximately 30 mm effective space with blind wound,
- rotating section begins approximately 24 mm from the right,
- programming button must remain accessible.

Decided:

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
- production FreeCAD macros must expose open dimensions instead of silently guessing them.

Open:

- exact slat geometry,
- exact stationary-holder geometry,
- final magnetic-section dimensions,
- final outer diameter,
- final M3 boss geometry,
- final magnet-pocket dimensions,
- final printing tolerances,
- final material and printing parameters,
- final CAD coordinate convention,
- detailed Hall sensor geometry.

## 2026-08-14 – Mechanical consolidation

### Added

- Created `measurements/measured_dimensions.md` as the consolidated measurement register.
- Consolidated the physical measurement note `images/roller_blind/rozmery.txt` with the previously confirmed dimensions.
- Documented the distinction between the nominal `8 × 20 mm` shaft note and the confirmed `16.70 mm across flats` dimension.
- Consolidated the FreeCAD development history from Rev.A through Rev.E5.
- Documented which CAD values are historical references rather than physical measurements.

### Superseded / clarified

- The previous 7.00 mm shaft-side value is superseded by the confirmed 6.70 mm slat feature within the 5 + 6.7 + 5 mm geometry.
- Octagonal internal geometry through the magnetic collar is superseded by the circular rotating clearance concept.
- The single-piece elastic-opening carrier concept is superseded by the approved two-piece carrier architecture.
- Axial magnet insertion is superseded by radial insertion from the outer circumference.

### Current mechanical status

The historical Rev.E5 macro is the latest CAD implementation currently stored, but it is **not declared the production revision**. The production candidate must first incorporate the approved two-piece architecture and the remaining OPEN physical dimensions.
