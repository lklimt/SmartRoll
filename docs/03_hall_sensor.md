# SmartRoll – Hall Sensor and Direction Detection

## Current approved concept

The rotating carrier carries two magnets positioned 180° apart. Hall sensing is intended to provide rotational position information and determine the direction in which the roller blind is moving.

## Current status

- Two magnets: **DECIDED**.
- Magnet separation: **180° – DECIDED**.
- Radial magnet orientation: **DECIDED**.
- Detailed Hall sensor placement: **OPEN**.
- Exact electrical circuit: **OPEN**.
- Firmware state machine: **OPEN**.

## Important design rule

Direction detection must be designed from the sequence of the two Hall signals, not inferred from a single Hall transition.

The final sensor geometry and signal truth table must be documented here before firmware implementation is frozen.
