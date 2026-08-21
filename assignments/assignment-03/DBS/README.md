# Dashboard Subsystem (DBS)

Java-only desktop dashboard for the Smart Tank Monitoring System.

## Runtime

The DBS is a Java Swing application. It does not use HTML, CSS, JavaScript, or external libraries.

Requirements:

- Java 17+
- CUS running and reachable over HTTP

Run from this directory:

```sh
gradle run
```

Configuration is in `config.properties`.

## HTTP API Used

DBS communicates with CUS via HTTP:

- `GET /api/state`
- `POST /api/mode`
- `POST /api/valve`

## Features

- Graph of the last `N` rainwater measurements.
- Current valve opening percentage.
- System state: `AUTOMATIC`, `MANUAL`, `UNCONNECTED`, or `NOT_AVAILABLE`.
- Mode switch controls.
- Manual valve slider enabled only in `MANUAL`.

