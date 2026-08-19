# Control Unit Subsystem (CUS)

PC backend implementation of the CUS, following the updated state diagrams.

## Runtime

The CUS is a Java application, not an Arduino/ESP32 sketch.

Requirements:

- Java 17
- Gradle
- MQTT broker reachable from the PC
- Serial port connected to the WCS Arduino

Run from this directory:

```sh
gradle run
```

Configuration is in `config.properties`.

## Architecture

- `MqttTask`: receives TMS telemetry via MQTT.
- `CusControlTask`: manages `AUTOMATIC`, `MANUAL`, `UNCONNECTED`.
- `ValveRegulationTask`: implements the automatic valve FSM: `CLOSED`, `SEMI-OPEN`, `OPEN`.
- `SerialTask`: communicates with WCS only through the serial line.
- `HttpTask`: exposes HTTP endpoints for DBS and uses Java `HttpClient` for optional HTTP push when `dbs.pushUrl` is configured.

## WCS Serial Protocol

CUS sends:

- `MODE AUTOMATIC`
- `MODE MANUAL`
- `MODE UNCONNECTED`
- `CLOSE`
- `SEMI-OPEN`
- `OPEN`
- `VALVE <0-100>`

CUS reads:

- `HELLO WCS`
- `MODE AUTOMATIC`
- `MODE MANUAL`
- `VALVE <0-100>`

## DBS HTTP API

`GET /api/state`

Returns current state:

```json
{"mode":"AUTOMATIC","waterLevel":42,"waterLevelAvailable":true,"valveOpening":50,"valveState":"SEMI-OPEN","mqttConnected":true,"wcsConnected":true,"httpAvailable":true}
```

`POST /api/mode`

Accepted payload examples:

```json
{"mode":"MANUAL"}
```

```json
{"mode":"AUTOMATIC"}
```

`POST /api/valve`

Accepted payload examples:

```json
{"valve":75}
```

```json
{"opening":75}
```

