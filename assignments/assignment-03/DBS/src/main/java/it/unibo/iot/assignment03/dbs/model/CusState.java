package it.unibo.iot.assignment03.dbs.model;

public record CusState(
    SystemMode mode,
    int waterLevel,
    boolean waterLevelAvailable,
    int valveOpening,
    String valveState,
    boolean mqttConnected,
    boolean wcsConnected,
    boolean httpAvailable
) {
    public static CusState notAvailable() {
        return new CusState(SystemMode.NOT_AVAILABLE, 0, false, 0, "UNKNOWN", false, false, false);
    }
}

