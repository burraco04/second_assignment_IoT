# Tank Monitoring Subsystem (TMS)

ESP32 implementation of the assignment TMS using synchronous finite state machines and a cooperative task scheduler.

## Architecture

- `NetworkTask` runs every 100 ms and keeps WiFi/MQTT connected.
- `MeasuringTask` runs every 500 ms, reads the sonar, computes the water level percentage, and publishes it to MQTT.
- `LedTask` runs every 500 ms and implements the LED FSM:
  - `Ok`: green LED on, red LED off.
  - `Error`: red LED on, green LED off.

## MQTT

The sketch uses `WiFi.h` and `PubSubClient.h`.

Published topics:

- `assignment03/tms/telemetry`
  - JSON payload: `{"seq":0,"ts":1234,"waterLevel":42,"distanceCm":28.50}`
- `assignment03/tms/status`
  - retained payload: `ok` or `error`
- `assignment03/tms/availability`
  - retained payload: `online`
  - MQTT last will: `offline`

## Configuration

Edit `config.h` before flashing:

- WiFi SSID/password.
- MQTT broker host/port and optional credentials.
- ESP32 pins for sonar and LEDs.
- Sonar calibration distances:
  - `SONAR_FULL_CM`: sensor-to-water distance when the tank is full.
  - `SONAR_EMPTY_CM`: sensor-to-water distance when the tank is empty.

## Expected libraries

Install these Arduino libraries:

- ESP32 board support package.
- `PubSubClient`.

