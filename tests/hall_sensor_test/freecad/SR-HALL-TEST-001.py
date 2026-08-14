# -*- coding: utf-8 -*-
"""
SmartRoll – SR-HALL-TEST-001
Adjustable Hall sensor / magnet test fixture.

Purpose:
- hold two Hall sensors A/B,
- provide adjustable angular spacing,
- provide a simple rotating magnet/carrier reference,
- allow practical air-gap experiments before the final carrier is frozen.

This is a TEST FIXTURE, not production geometry.

Open candidate parameters are collected at the top so the fixture can be
changed without rewriting the geometry.
"""

import FreeCAD as App
import Part
import math

DOC = "SR-HALL-TEST-001"

# ============================================================
# CANDIDATE / ADJUSTABLE PARAMETERS
# ============================================================

BASE_X = 80.0
BASE_Y = 50.0
BASE_Z = 6.0

SENSOR_BLOCK_X = 12.0
SENSOR_BLOCK_Y = 8.0
SENSOR_BLOCK_Z = 4.0

SENSOR_CENTER_RADIUS = 25.0
SENSOR_SLOT_LENGTH = 20.0
SENSOR_SLOT_WIDTH = 3.5

# Starting hypothesis only – measure experimentally.
START_ANGLE = 45.0

# Test magnet carrier
TEST_CARRIER_OD = 40.0
TEST_CARRIER_ID = 10.0
TEST_CARRIER_THICKNESS = 5.0

MAGNET_D = 4.0
MAGNET_L = 2.0

# ============================================================
# HELPERS
# ============================================================

def box(x, y, z, p):
    return Part.makeBox(x, y, z, App.Vector(*p))


def cylinder(r, h, p, axis=(0,0,1)):
    return Part.makeCylinder(r, h, App.Vector(*p), App.Vector(*axis))


def polar_xy(r, angle_deg, z):
    a = math.radians(angle_deg)
    return App.Vector(r * math.cos(a), r * math.sin(a), z)


def feature(doc, name, shape, label=None):
    o = doc.addObject("PartDesign::Feature", name)
    o.Shape = shape
    o.Label = label or name
    return o

# ============================================================
# DOCUMENT
# ============================================================

doc = App.newDocument(DOC)

# Parameter sheet
sheet = doc.addObject("Spreadsheet::Sheet", "TestParameters")
params = [
    ("PROJECT", "SmartRoll"),
    ("FIXTURE", "SR-HALL-TEST-001"),
    ("STATUS", "TEST FIXTURE"),
    ("START_SENSOR_ANGLE", START_ANGLE),
    ("SENSOR_CENTER_RADIUS", SENSOR_CENTER_RADIUS),
    ("TEST_CARRIER_OD", TEST_CARRIER_OD),
    ("TEST_CARRIER_ID", TEST_CARRIER_ID),
    ("MAGNET_D", MAGNET_D),
    ("MAGNET_L", MAGNET_L),
]
for i, (k,v) in enumerate(params, 1):
    sheet.set("A%d" % i, k)
    sheet.set("B%d" % i, str(v))

# Base plate with grid-like reference holes
base = box(BASE_X, BASE_Y, BASE_Z, (-BASE_X/2, -BASE_Y/2, 0))
feature(doc, "BasePlate", base, "Base plate")

# Central rotating carrier reference
carrier = cylinder(TEST_CARRIER_OD/2, TEST_CARRIER_THICKNESS,
                    (0,0,BASE_Z), (0,0,1))
carrier = carrier.cut(cylinder(TEST_CARRIER_ID/2, TEST_CARRIER_THICKNESS+1,
                               (0,0,BASE_Z-0.5), (0,0,1)))
feature(doc, "TestCarrier", carrier, "Test carrier – rotate manually")

# Two reference magnet pockets 180° apart. These are only visual reference
# features; actual magnets can be taped/glued temporarily for experiments.
for idx, ang in enumerate((0.0, 180.0), 1):
    p = polar_xy(TEST_CARRIER_OD/2 - MAGNET_D/2 - 1.0, ang, BASE_Z)
    pocket = cylinder(MAGNET_D/2, MAGNET_L+0.2,
                      (p.x,p.y,BASE_Z-0.1), (0,0,1))
    carrier_cut = carrier.cut(pocket)
    # Keep pocket as separate reference solid to make placement obvious.
    feature(doc, "MagnetReference%d" % idx, pocket,
            "Magnet %d position – 180° pair" % idx)

# Sensor A/B blocks. Their angle is represented by placement of blocks.
# For easy experimentation, duplicate the macro with a different
# START_ANGLE or adjust the objects interactively.
for idx, ang in enumerate((0.0, START_ANGLE), 1):
    p = polar_xy(SENSOR_CENTER_RADIUS, ang, BASE_Z)
    block = box(SENSOR_BLOCK_X, SENSOR_BLOCK_Y, SENSOR_BLOCK_Z,
                (p.x-SENSOR_BLOCK_X/2, p.y-SENSOR_BLOCK_Y/2, BASE_Z))
    obj = feature(doc, "HallSensor%d" % idx, block,
                  "Hall %s – adjustable position" % ("A" if idx == 1 else "B"))
    obj.addProperty("App::PropertyAngle", "Angle", "Test")
    obj.Angle = ang
    obj.addProperty("App::PropertyLength", "Radius", "Test")
    obj.Radius = SENSOR_CENTER_RADIUS

# A visual angular reference ring
ring = cylinder(SENSOR_CENTER_RADIUS+1.0, 1.0,
                (0,0,BASE_Z+SENSOR_BLOCK_Z+0.5), (0,0,1))
ring = ring.cut(cylinder(SENSOR_CENTER_RADIUS-1.0, 1.2,
                         (0,0,BASE_Z+SENSOR_BLOCK_Z+0.4), (0,0,1)))
feature(doc, "AngularReference", ring, "Angular reference")

# Notes object
notes = doc.addObject("App::FeaturePython", "TestNotes")
notes.addProperty("App::PropertyString", "Purpose", "Documentation")
notes.Purpose = "Find practical Hall-to-magnet gap and A/B phase spacing."
notes.addProperty("App::PropertyString", "StartingHypothesis", "Documentation")
notes.StartingHypothesis = "45° sensor separation is only a starting hypothesis."
notes.addProperty("App::PropertyString", "Status", "Documentation")
notes.Status = "TEST FIXTURE – NOT PRODUCTION GEOMETRY"

# Keep default colors; no assumptions about final appearance.
doc.recompute()

print("SR-HALL-TEST-001 created")
print("Starting Hall A/B angle: %.1f deg" % START_ANGLE)
print("Test carrier: %.1f mm OD / %.1f mm ID" % (TEST_CARRIER_OD, TEST_CARRIER_ID))
print("Magnet target: %.1f x %.1f mm" % (MAGNET_D, MAGNET_L))
print("STATUS: TEST FIXTURE")
