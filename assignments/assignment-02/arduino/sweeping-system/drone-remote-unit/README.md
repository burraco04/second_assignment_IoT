# Drone Remote Unit

Java Swing GUI for the Smart Drone Hangar assignment.

## Requirements

- Windows
- JDK 21 or newer in `PATH`
- Arduino connected over a COM serial port at `115200` baud
- `lib/jssc-2.9.4.jar`

The serial connection is handled through JSSC.

## Run

Build the executable jar:

```powershell
cd drone-remote-unit
$sources = Get-ChildItem -Recurse src -Filter *.java
New-Item -ItemType Directory -Force build/classes
javac -encoding UTF-8 -cp lib/jssc-2.9.4.jar -d build/classes $sources.FullName
jar cfm drone-remote-unit.jar MANIFEST.MF -C build/classes .
```

Run:

```powershell
cd drone-remote-unit
java -jar drone-remote-unit.jar
```

## Serial Protocol

Commands sent to Arduino:

- `TAKEOFF`
- `LAND`
- `RESET`
- custom raw command from the text field

Messages received from Arduino:

- `STATE:RESTING`
- `STATE:TAKING_OFF`
- `STATE:FLYING`
- `STATE:LANDING`
- `STATE:SUSPENDED`
- `HANGAR:NORMAL`
- `HANGAR:PRE_ALARM`
- `HANGAR:ALARM`
- `DISTANCE:<meters>`

The GUI also displays Arduino log lines prefixed by `lo:`.
