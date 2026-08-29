# SmartRoll – Test 4 Results

## ESP32 + Hall A/B + ERTE RF

## Test A – stationary baseline

Result: ___

## Test B – RF UP + Hall tracking

Result: ___

## Test C – RF DOWN + Hall tracking

Evidence: `evidence/esp32_hall_rf_testC.txt`

**PASS**

The DOWN command was transmitted from the ESP32 and the Hall quadrature decoder tracked the movement continuously.

Observed sequence:

```text
TX 01A DOWN : 33 63 13 79 19
RF send repetitions: 4
```

Hall tracking during the movement:

```text
Position:  -8  CW=0  CCW=8  Invalid=0
Position: -12  CW=0  CCW=12  Invalid=0
Position: -16  CW=0  CCW=16  Invalid=0
Position: -20  CW=0  CCW=20  Invalid=0
...
Position: -32  CW=0  CCW=32  Invalid=0
...
Position: -40  CW=0  CCW=40  Invalid=0
```

The complete observed movement produced **40 CCW transitions and 0 CW transitions**, with **0 invalid transitions**. The STOP command was then transmitted successfully and the Hall count remained stable while stationary. The subsequent movement continued in the same decoded direction without errors.

This confirms that RF transmission and Hall quadrature decoding can operate together on the ESP32 during real roller movement.

## Test D – repeated UP/STOP/DOWN cycles

Result: ___

## Test E – RF commands during Hall activity

Result: ___

## Test F – ESP32 reset

Result: ___

## Overall result

**IN PROGRESS – Test 4C PASS.**

Evidence files are stored in this directory under `evidence/`.
