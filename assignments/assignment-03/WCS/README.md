# Water Channel Subsystem (WCS)

Arduino UNO implementation of the WCS using synchronous finite state machines and a cooperative task scheduler.

## Architecture

- `CusTask`: serial communication with CUS, connection timeout, mode acknowledgements, valve commands.
- `ButtonTask`: debounced local button FSM, toggles `MANUAL` / `AUTOMATIC` requests.
- `PotentiometerTask`: sleeps outside `MANUAL`, samples the potentiometer in `MANUAL`.
- `ValveTask`: maps opening percentage `0..100` to servo angle `0..90`.
- `LcdTask`: displays current mode and valve opening.

`WCS.cpp` only creates shared devices/state, registers tasks in the scheduler, and exposes `setup()` / `loop()`.

## Serial Protocol

Incoming commands from CUS:

- `MODE AUTOMATIC`
- `MODE MANUAL`
- `MODE UNCONNECTED`
- `VALVE <0-100>`
- `OPENING <0-100>`
- `STATE <MODE> <0-100>`
- `PING`

Outgoing messages to CUS:

- `HELLO WCS`
- `MODE AUTOMATIC`
- `MODE MANUAL`
- `VALVE <0-100>`
- `PONG`

If no serial message is received for `CUS_TIMEOUT_MS`, the WCS enters `UNCONNECTED` and closes the valve.

