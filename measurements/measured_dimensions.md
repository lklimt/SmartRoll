# SmartRoll – Measured Dimensions

This file is the consolidated measurement register for the mechanical SmartRoll design.

## Status legend

- **CONFIRMED** – explicitly measured/confirmed by the user or supported by the measurement record.
- **DECIDED** – design decision, not a physical measurement.
- **REFERENCE** – useful reference value from an existing CAD revision; not automatically a physical measurement.
- **OPEN** – must still be measured or confirmed.
- **SUPERSEDED** – historical value replaced by a newer confirmed value.

## 1. Roller blind / installation

| Parameter | Value | Status | Evidence / note |
|---|---:|---|---|
| Application | External roller blind | CONFIRMED | Project specification |
| Motor | ERTE ET 45E | CONFIRMED | Project specification |
| Roller shaft | Octagonal | CONFIRMED | Photos + previous measurements |
| Shaft nominal description | 8 × 17 mm | REFERENCE | `images/roller_blind/rozmery.txt`; this is not the same definition as AF |
| Shaft across flats | 16.70 mm | CONFIRMED | Explicitly measured/confirmed during design |
| Slat-side total width | 5 + 7.0 + 5 mm = 17.0 mm | CONFIRMED | Explicitly corrected during design |
| Slat feature width | 6.70 mm | CONFIRMED | Explicitly corrected during design; previous 7 mm is obsolete |
| Cabinet depth | 160 mm | CONFIRMED | `rozmery.txt` |
| Cabinet height | 160 mm | CONFIRMED | `rozmery.txt` |
| Wound blind distance from wall | approx. 30 mm | CONFIRMED | `rozmery.txt` and design discussion |
| Unwound available space | approx. 35 mm | CONFIRMED | Direct design measurement |
| Effective wound clearance | approx. 30 mm | CONFIRMED | Critical design condition |
| Rotating section starts from right | approx. 24 mm | CONFIRMED / APPROXIMATE | Direct measurement/discussion |
| Programming button | Must remain accessible | DECIDED | Right-side stationary motor part |

## 2. Carrier geometry – approved design decisions

| Parameter | Value | Status | Note |
|---|---:|---|---|
| Carrier architecture | Two-piece | DECIDED | Two halves installed independently |
| Clamping width | 6.00 mm | DECIDED | Available shaft attachment zone |
| Clamping profile | Octagonal | DECIDED | Must engage shaft positively |
| Magnetic collar width | 5.00 mm | DECIDED | Total carrier width 11 mm |
| Magnetic internal clearance | Circular | DECIDED | Must rotate around stationary part |
| Magnet count | 2 | DECIDED | Opposed pair |
| Magnet angular spacing | 180° | DECIDED | Opposed pair |
| Magnet orientation | Radial | DECIDED | Inserted from outer circumference |
| Magnet nominal size | 4 × 2 mm | DECIDED | Final magnet orientation/dimensions to be verified against purchased parts |
| M3 connections | 2 | DECIDED | Used to clamp two carrier halves |
| Slat clearance orientation | 90° from joining arrangement | DECIDED | Chosen to avoid putting the slat feature at a split |

## 3. Historical CAD-only values – do not treat as confirmed

The following values appear in previous FreeCAD revisions and are therefore useful for reconstructing design history, but they are **not physical measurements unless independently confirmed**:

| Parameter | Historical value | Revision(s) | Status |
|---|---:|---|---|
| Carrier outer diameter | Ø26.00 mm | E1/E2/E5 | REFERENCE |
| Magnetic circular clearance | Ø21.00 mm | E1/E2/E5 | REFERENCE |
| Shaft clearance added in CAD | 0.20 mm | E1/E2/E5 | REFERENCE / design tolerance |
| M3 through-hole | Ø3.30 mm | E2/E5 | REFERENCE |
| M3 head recess | 1.20–2.30 mm depending revision | E2/E5 | SUPERSEDED / OPEN |
| Nut AF | 5.50 mm | E2/E5 | REFERENCE |
| Magnet pocket | Ø4.20 mm × approx. 2.15 mm | E1/E2/E5 | REFERENCE |
| Clamp slot width | 0.80 mm | E1/E2/E5 | REFERENCE |

## 4. Important correction history

### Shaft side dimension

A previous CAD iteration used **7.00 mm** for one shaft side. This is superseded.

The confirmed geometry is:

`5.0 + 6.7 + 5.0 = 16.7 mm`

The 6.70 mm slat feature must therefore not be silently replaced by 7.00 mm in future macros.

### Magnetic collar internal profile

Early concepts incorrectly retained octagonal internal geometry through the magnetic collar. The approved concept is:

- 0–6 mm: octagonal shaft-clamping bore;
- 6–11 mm: circular internal clearance.

The reason is that the magnetic collar rotates around the stationary motor-holder geometry.

### Carrier installation

The original single-slit ring concept was rejected as potentially impractical to open sufficiently in tough ABS.

The approved architecture is two separate halves joined by two M3 screws.

## 5. Open measurements required before production macro

The following must be measured or confirmed before a production-ready FreeCAD model is released:

1. Exact external dimensions of the stationary black motor-holder.
2. Exact diameter/profile of the part around which the magnetic collar must rotate.
3. Exact axial position and usable 6 mm clamping zone relative to the right-side end.
4. Exact programming-button position and required access envelope.
5. Exact slat feature depth and cross-section.
6. Exact minimum clearance in the fully wound condition at the carrier location.
7. Exact magnet dimensions of the selected physical magnets.
8. Exact Hall sensor position relative to the magnet path.
9. Required 3D-printing clearance for the shaft fit.

## 6. Evidence files

Primary measurement evidence currently stored in:

`images/roller_blind/`

Relevant files include:

- `Detail_zamereni_1.jpeg` … `Detail_zamereni_6.jpeg`
- `pravy-rozvinuto-detail.jpeg`
- `pravy-navinuto-detail.jpeg`
- `pravy-bocni pohled-navinuto.jpeg`
- `pravy-rozvinuto-situace.jpeg`
- `levy-rozvinuto.jpeg`
- `rozmery.txt`

Operational reference videos:

- `navijeni-levy-video.MOV`
- `navijeni-pravy-video.MOV`
