# Water Channel Subsystem (WCS)

Arduino UNO implementation of the WCS using synchronous finite state machines and the shared cooperative task scheduler in `../common`.

## Architecture

- `SerialTask`: serial communication with CUS, mode acknowledgements, valve commands.
- `ButtonTask`: debounced local button FSM, toggles `MANUAL` / `AUTOMATIC` requests.
- `PotentiometerTask`: sleeps outside `MANUAL`, samples the potentiometer in `MANUAL`.
- `ValveTask`: maps opening percentage `0..100` to servo angle `0..90` using `devices/ServoMotorImpl`.
- `LcdTask`: displays current mode and valve opening on an I2C LCD.

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

## LCD Wiring

The LCD is the I2C version:

- `GND` -> Arduino `GND`
- `VCC` -> Arduino `5V`
- `SDA` -> Arduino UNO `A4`
- `SCL` -> Arduino UNO `A5`

Default I2C address is `0x27` in `config.h`. If the display does not show text, try `0x3F`.
