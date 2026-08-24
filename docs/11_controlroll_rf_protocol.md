# SmartRoll – Frozen ControlRoll RF Protocol

**Status:** FROZEN REFERENCE / migration to ESP32 pending verification

## 1. Purpose

The RF protocol used by the existing `ControlRoll` project was obtained by reverse engineering the ERTE roller-blind RF control and was subsequently validated on the Arduino-based implementation.

This protocol is now **frozen as a project reference**.

SmartRoll shall **not re-invent, reinterpret or modify the ERTE RF protocol** during migration to ESP32.

The task is to reproduce the already validated waveform and data on the new MCU.

## 2. Existing implementation

The existing ControlRoll firmware uses the `RF433send` library and the `TRIBIT` encoding.

The library version used by the project is compatible with AVR, ESP8266 and ESP32. The library metadata explicitly lists:

```text
architectures=avr, esp32, esp8266
```

Therefore the library itself does not require a protocol redesign for ESP32.

## 3. ERTE command data

The existing ControlRoll command data remains the authoritative source for the roller commands:

```text
UP
STOP
DOWN
```

The per-roller byte sequences already verified in ControlRoll shall be copied unchanged into SmartRoll firmware.

No bit order, byte order, checksum interpretation or command value shall be changed unless a new physical test proves the existing implementation incorrect.

## 4. RF433send timing reference

The validated ControlRoll implementation uses `RfSendEncoding::TRIBIT` with the following timing parameters:

```text
initseq       = 9712 µs
lo_prefix     = 5048 µs
hi_prefix     = 1472 µs
first_lo_ign  = unused for TRIBIT
lo_short      = 362 µs
lo_long       = 722 µs
hi_short      = 704 µs
hi_long       = 0 / library default where applicable
lo_last       = 0 / library parameter
sep           = 9360 µs
nb_bits       = 39
```

The exact constructor arguments from the working ControlRoll firmware are the authoritative implementation reference. Do not reconstruct these values from rounded documentation.

## 5. Library implementation

`RF433send` generates the signal using Arduino-compatible `digitalWrite()` and microsecond delays. The library's ESP32 support is explicitly declared in its `library.properties`.

The SmartRoll migration shall therefore first attempt to use the same library and the same TRIBIT parameters on ESP32.

A different ESP32 RF implementation (for example hardware-timed RMT) may only replace it after the generated waveform has been compared against the validated ControlRoll waveform and the ERTE motor has been tested successfully.

## 6. Migration acceptance test

The ESP implementation is accepted only when all of the following are true:

1. UP command operates the ERTE motor correctly.
2. STOP command operates correctly.
3. DOWN command operates correctly.
4. The correct roller addressing is preserved.
5. Repeated transmissions are reliable.
6. The RF waveform timing is consistent with the ControlRoll reference.
7. RF transmission does not cause Hall A/B quadrature transitions to be lost.
8. RF transmission does not cause ESP watchdog resets or unexpected reboots.
9. Wi-Fi remains operational.

## 7. RF + Hall concurrency requirement

The most important SmartRoll-specific RF test is simultaneous operation:

```text
Hall A/B rotation
        +
433 MHz RF transmission
        +
Wi-Fi active
```

The test shall verify:

- no missed Hall transitions,
- no false direction changes,
- no unexpected invalid transitions,
- no RF command corruption,
- no ESP reset.

The result of this test determines whether the existing blocking RF433send implementation is acceptable on the selected ESP platform.

## 8. Controller decision

The project shall proceed with **ESP32 as the preferred final controller platform**, because it provides substantially more processing/peripheral headroom for Hall decoding, RF, Wi-Fi and the planned sensors.

The existing Wemos D1 mini / ESP8266 remains useful as a development and fallback platform.

The final controller is not considered frozen until the RF + Hall concurrency test passes.

## 9. Important rule

The following are now considered **FROZEN** from ControlRoll:

```text
ERTE RF command data
TRIBIT encoding
validated timing parameters
command semantics UP / STOP / DOWN
```

Only the MCU implementation is being changed.
