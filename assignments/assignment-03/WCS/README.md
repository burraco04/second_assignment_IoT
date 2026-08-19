# Water Channel Subsystem (WCS)

Arduino UNO implementation of the WCS using synchronous finite state machines and the shared cooperative task scheduler in `../common`.

## Architecture

- `SerialTask`: serial communication with CUS, mode acknowledgements, valve commands.
- `ButtonTask`: debounced local button FSM, toggles `MANUAL` / `AUTOMATIC` requests.
- `PotentiometerTask`: sleeps outside `MANUAL`, samples the potentiometer in `MANUAL`.
- `ValveTask`: maps opening percentage `0..100` to servo angle `0..90`.
- `LcdTask`: displays current mode and valve opening.

`WCS.cpp` only creates shared devices/state, registers tasks in the scheduler, and exposes `setup()` / `loop()`.

## Serial Protocol

Incoming commands from CUS:

- `OPEN`
- `SEMI-OPEN`
- `CLOSE`
- `MODE AUTOMATIC`
- `MODE MANUAL`
- `MODE UNCONNECTED`
- `VALVE <0-100>`
- `OPENING <0-100>`
- `PING`

Outgoing messages to CUS:

- `HELLO WCS`
- `MODE AUTOMATIC`
- `MODE MANUAL`
- `VALVE <0-100>`
- `PONG`

In `AUTOMATIC`, `OPEN` / `SEMI-OPEN` / `CLOSE` drive the valve. In `MANUAL`, the potentiometer drives the valve and WCS reports `VALVE <0-100>` to CUS.
