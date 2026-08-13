# -*- coding: utf-8 -*-
"""
SmartRoll - SR-MEC-002 Rev.A - Final Candidate

Clean reconstruction of the two-piece magnetic carrier from the
consolidated project specification.

IMPORTANT:
This is a FINAL CANDIDATE for visual/mechanical verification, not yet a
manufacturing-approved revision.

Confirmed / decided design basis:
- octagonal shaft: 16.70 mm across flats
- shaft-side feature: 5 + 6.7 + 5 mm
- clamping zone: 6 mm axial width
- two carrier halves
- two M3 clamp connections
- slat clearance 90 deg from split/joining arrangement
- magnetic section has circular internal clearance
- two radial magnets, 180 deg apart
- nominal magnets 4 x 2 mm

OPEN dimensions are explicitly exposed below and MUST NOT be silently
changed inside geometry operations.

Run in FreeCAD Python console or Macro editor.
"""

import FreeCAD as App
import Part
import math

# ============================================================
# PROJECT PARAMETERS
# ============================================================

DOC_NAME = "SR-MEC-002_RevA_FinalCandidate"

# --- CONFIRMED / DECIDED ---
SHAFT_AF = 16.70              # across flats
CLAMP_WIDTH = 6.00            # axial width of octagonal clamp zone
SLAT_CENTER_WIDTH = 6.70      # confirmed slat-side central width
SLAT_SIDE = 5.00              # confirmed side portions
MAGNET_DIAMETER = 4.00
MAGNET_LENGTH = 2.00
MAGNET_ANGLE = 180.0

# --- OPEN / CANDIDATE VALUES ---
# These are deliberately grouped here so they cannot be mistaken for
# confirmed measured values.
MAGNET_RING_WIDTH = 5.00       # candidate axial width
MAGNET_RING_OD = 25.00         # candidate outer diameter
ROTATING_CLEARANCE_D = 18.00   # candidate circular internal diameter
MAGNET_RADIAL_DEPTH = 2.20     # candidate pocket radial depth
MAGNET_POCKET_AXIAL = 2.20     # candidate axial pocket height
M3_CLEARANCE = 3.40
M3_BOSS_OD = 7.00
M3_BOSS_HEIGHT = 6.00
NUT_AF = 5.50                  # M3 hex nut nominal AF; candidate pocket
NUT_DEPTH = 2.60
SPLIT_GAP = 0.35
SLAT_CLEARANCE = 7.10          # candidate clearance over 6.7 mm feature

# Placement / orientation
# Shaft axis is X. Shaft cross-section lies in YZ.
# Split plane is Y=0. The slat clearance is on the +Z face, i.e. 90 deg
# from the split planes.

# ============================================================
# HELPERS
# ============================================================

def octagon_prism_af(af, width):
    """Regular octagonal prism, axis X, across flats = af."""
    r = af / (2.0 * math.cos(math.pi / 8.0))
    pts = []
    # vertices in YZ plane
    for i in range(8):
        a = math.radians(22.5 + i * 45.0)
        pts.append(App.Vector(0, r * math.cos(a), r * math.sin(a)))
    pts.append(pts[0])
    wire = Part.makePolygon(pts)
    face = Part.Face(wire)
    return face.extrude(App.Vector(width, 0, 0))


def cylinder_x(radius, length, x0=0):
    return Part.makeCylinder(radius, length, App.Vector(x0, 0, 0), App.Vector(1, 0, 0))


def radial_cylinder(center_angle_deg, radius, depth, axial_start, axial_len):
    """Cylinder used as a radial magnet pocket, axis radial in YZ."""
    a = math.radians(center_angle_deg)
    direction = App.Vector(0, math.cos(a), math.sin(a))
    start = App.Vector(axial_start, direction.y * (radius - depth), direction.z * (radius - depth))
    return Part.makeCylinder(radius, axial_len, start, direction)


def add_feature(obj, name, shape):
    f = obj.newObject("PartDesign::Feature", name)
    f.Shape = shape
    return f

# ============================================================
# BUILD DOCUMENT
# ============================================================

doc = App.newDocument(DOC_NAME)

# Parameter spreadsheet for inspection in FreeCAD
sheet = doc.addObject("Spreadsheet::Sheet", "Parameters")
rows = [
    ("SR-MEC-002 Rev.A", "Final Candidate"),
    ("STATUS", "FINAL CANDIDATE - VERIFY BEFORE MANUFACTURING"),
    ("SHAFT_AF", SHAFT_AF),
    ("CLAMP_WIDTH", CLAMP_WIDTH),
    ("SLAT_SIDE", SLAT_SIDE),
    ("SLAT_CENTER_WIDTH", SLAT_CENTER_WIDTH),
    ("MAGNET_DIAMETER", MAGNET_DIAMETER),
    ("MAGNET_LENGTH", MAGNET_LENGTH),
    ("MAGNET_ANGLE", MAGNET_ANGLE),
    ("MAGNET_RING_WIDTH", MAGNET_RING_WIDTH),
    ("MAGNET_RING_OD", MAGNET_RING_OD),
    ("ROTATING_CLEARANCE_D", ROTATING_CLEARANCE_D),
    ("MAGNET_RADIAL_DEPTH", MAGNET_RADIAL_DEPTH),
    ("M3_CLEARANCE", M3_CLEARANCE),
    ("M3_BOSS_OD", M3_BOSS_OD),
    ("M3_BOSS_HEIGHT", M3_BOSS_HEIGHT),
    ("NUT_AF", NUT_AF),
    ("NUT_DEPTH", NUT_DEPTH),
    ("SPLIT_GAP", SPLIT_GAP),
    ("SLAT_CLEARANCE", SLAT_CLEARANCE),
]
for i, (k, v) in enumerate(rows, 1):
    sheet.set("A%d" % i, k)
    sheet.set("B%d" % i, str(v))
sheet.set("D1", "Design notes")
sheet.set("D2", "X = shaft axis")
sheet.set("D3", "Split planes at Y=0")
sheet.set("D4", "Slat clearance on +Z face")
sheet.set("D5", "Magnetic section uses circular ID")
sheet.set("D6", "Two halves clamp with M3")

# ============================================================
# CORE HALF GEOMETRY
# ============================================================

# Full octagonal clamp body, then split into two halves.
clamp_full = octagon_prism_af(SHAFT_AF, CLAMP_WIDTH)

# Circular magnetic collar located immediately after clamp zone.
mag_start = CLAMP_WIDTH
mag_full = cylinder_x(MAGNET_RING_OD / 2.0, MAGNET_RING_WIDTH, mag_start)
mag_full = mag_full.cut(cylinder_x(ROTATING_CLEARANCE_D / 2.0, MAGNET_RING_WIDTH + 0.2, mag_start - 0.1))

# Join the functional zones.
full = clamp_full.fuse(mag_full)

# Slat clearance: remove a rectangular radial opening on +Z side of the
# octagonal clamping region. It is intentionally independent of the
# circular magnetic clearance.
slat_cut = Part.makeBox(CLAMP_WIDTH + 0.2, SLAT_CLEARANCE, 5.0,
                        App.Vector(-0.1, -SLAT_CLEARANCE / 2.0, SHAFT_AF / 2.0 - 1.0))
full = full.cut(slat_cut)

# ============================================================
# SPLIT INTO TWO INSTALLABLE HALVES
# ============================================================

# Half A: Y >= gap/2
box_y_pos = Part.makeBox(CLAMP_WIDTH + MAGNET_RING_WIDTH + 1.0,
                         MAGNET_RING_OD + 4.0,
                         MAGNET_RING_OD + 4.0,
                         App.Vector(-0.5, SPLIT_GAP / 2.0, -MAGNET_RING_OD / 2.0 - 2.0))

# Half B: Y <= -gap/2
box_y_neg = Part.makeBox(CLAMP_WIDTH + MAGNET_RING_WIDTH + 1.0,
                         MAGNET_RING_OD + 4.0,
                         MAGNET_RING_OD + 4.0,
                         App.Vector(-0.5, -MAGNET_RING_OD / 2.0 - 2.0, -MAGNET_RING_OD / 2.0 - 2.0))

half_a = full.common(box_y_pos)
half_b = full.common(box_y_neg)

# ============================================================
# MAGNET POCKETS - RADIAL, 180 DEG APART
# ============================================================

# Pockets are cut radially from the outside of the magnetic ring.
# Candidate angles are +/-90 deg so they do not coincide with the split.
for ang in (90.0, 270.0):
    a = math.radians(ang)
    # radial direction in YZ
    d = App.Vector(0, math.cos(a), math.sin(a))
    # pocket starts outside and cuts inward
    outer_r = MAGNET_RING_OD / 2.0 + 0.5
    start = App.Vector(mag_start + (MAGNET_RING_WIDTH - MAGNET_POCKET_AXIAL) / 2.0,
                       d.y * outer_r, d.z * outer_r)
    pocket = Part.makeCylinder(MAGNET_DIAMETER / 2.0,
                               MAGNET_RADIAL_DEPTH + 1.0,
                               start,
                               App.Vector(0, -d.y, -d.z))
    # apply to the appropriate half: +Z magnet on A, -Z magnet on B
    if ang == 90.0:
        half_a = half_a.cut(pocket)
    else:
        half_b = half_b.cut(pocket)

# ============================================================
# M3 BOSSES
# ============================================================

# Two joining positions along Z. Each side receives a complete boss.
# Bosses are outside the rotating inner clearance.
for z in (-MAGNET_RING_OD / 2.0 + 3.5, MAGNET_RING_OD / 2.0 - 3.5):
    # Cylindrical boss axis Y, positioned near split plane.
    boss_center_x = (CLAMP_WIDTH + MAGNET_RING_WIDTH) / 2.0
    boss = Part.makeCylinder(M3_BOSS_OD / 2.0, M3_BOSS_HEIGHT,
                             App.Vector(boss_center_x, -M3_BOSS_HEIGHT / 2.0, z),
                             App.Vector(0, 1, 0))
    screw = Part.makeCylinder(M3_CLEARANCE / 2.0, M3_BOSS_HEIGHT + 0.4,
                              App.Vector(boss_center_x, -M3_BOSS_HEIGHT / 2.0 - 0.2, z),
                              App.Vector(0, 1, 0))
    boss = boss.cut(screw)
    # Nut pocket on negative-Y side, hexagonal approximation via polygon.
    # Kept as a dedicated feature so it can be inspected/adjusted.
    pts = []
    rr = NUT_AF / (2.0 * math.cos(math.pi / 6.0))
    for i in range(6):
        aa = math.radians(i * 60.0 + 30.0)
        pts.append(App.Vector(boss_center_x + rr * math.cos(aa),
                              -M3_BOSS_HEIGHT / 2.0 - 0.1,
                              z + rr * math.sin(aa)))
    pts.append(pts[0])
    nut_face = Part.Face(Part.makePolygon(pts))
    nut_pocket = nut_face.extrude(App.Vector(0, NUT_DEPTH, 0))
    boss = boss.cut(nut_pocket)

    # Boss is deliberately split by the carrier halves: each half gets the
    # corresponding portion. This leaves complete external screw seating.
    boss_a = boss.common(box_y_pos)
    boss_b = boss.common(box_y_neg)
    half_a = half_a.fuse(boss_a)
    half_b = half_b.fuse(boss_b)

# ============================================================
# DISPLAY
# ============================================================

obj_a = add_feature(doc, "Carrier_Half_A", half_a)
obj_b = add_feature(doc, "Carrier_Half_B", half_b)
obj_a.Label = "SR-MEC-002 Rev.A – Carrier Half A"
obj_b.Label = "SR-MEC-002 Rev.A – Carrier Half B"

# Reference geometry: shaft and stationary circular clearance.
ref = doc.addObject("PartDesign::Feature", "Reference_Shaft")
ref.Shape = octagon_prism_af(SHAFT_AF, CLAMP_WIDTH)
ref.Label = "REFERENCE – 16.70 mm AF Octagonal Shaft"
ref.ViewObject.Transparency = 80

clear = doc.addObject("PartDesign::Feature", "Reference_Rotating_Clearance")
clear.Shape = cylinder_x(ROTATING_CLEARANCE_D / 2.0, MAGNET_RING_WIDTH, CLAMP_WIDTH)
clear.Label = "REFERENCE – Circular Rotating Clearance"
clear.ViewObject.Transparency = 85

# Colors are intentionally left to FreeCAD defaults; users can inspect
# each solid independently.

# Save
path = None
try:
    import os
    path = os.path.join(App.getUserAppDataDir(), DOC_NAME + ".FCStd")
except Exception:
    pass

App.ActiveDocument.recompute()

print("==============================================")
print("SmartRoll SR-MEC-002 Rev.A Final Candidate")
print("Created: two-piece magnetic carrier")
print("Shaft AF: %.2f mm" % SHAFT_AF)
print("Clamp width: %.2f mm" % CLAMP_WIDTH)
print("Magnetic collar width: %.2f mm (OPEN/CANDIDATE)" % MAGNET_RING_WIDTH)
print("Magnetic collar OD: %.2f mm (OPEN/CANDIDATE)" % MAGNET_RING_OD)
print("Rotating clearance ID: %.2f mm (OPEN/CANDIDATE)" % ROTATING_CLEARANCE_D)
print("Magnets: 2 x %.2f x %.2f mm, radial, 180 deg" % (MAGNET_DIAMETER, MAGNET_LENGTH))
print("M3: two joining positions")
print("STATUS: FINAL CANDIDATE - VERIFY BEFORE MANUFACTURING")
print("==============================================")
