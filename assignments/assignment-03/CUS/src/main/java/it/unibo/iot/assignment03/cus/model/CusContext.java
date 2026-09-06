package it.unibo.iot.assignment03.cus.model;

public final class CusContext {
    private SystemMode mode = SystemMode.AUTOMATIC;
    private ValveState valveState = ValveState.CLOSED;
    private int valveOpening = 0;
    private int manualOpening = 0;
    private int waterLevel = 0;
    private boolean waterLevelAvailable;
    private boolean levelTimerSet;
    private boolean modeChanged = true;
    private boolean valveStateChanged = true;
    private boolean manualValveChanged;
    private boolean mqttConnected;
    private boolean wcsConnected;
    private boolean httpAvailable;
    private long lastTmsMessage;
    private long waterLevelTimestamp;
    private SystemMode requestedMode;
    private Integer requestedManualOpening;

    public synchronized Snapshot snapshot() {
        return new Snapshot(
            mode,
            valveState,
            valveOpening,
            manualOpening,
            waterLevel,
            waterLevelAvailable,
            levelTimerSet,
            mqttConnected,
            wcsConnected,
            httpAvailable,
            lastTmsMessage,
            waterLevelTimestamp
        );
    }

    public synchronized void onWaterLevel(final int value, final long now) {
        waterLevel = clampPercentage(value);
        waterLevelAvailable = true;
        lastTmsMessage = now;
    }

    public synchronized void setMode(final SystemMode nextMode) {
        if (mode == nextMode) {
            return;
        }
        mode = nextMode;
        modeChanged = true;
        if (nextMode == SystemMode.MANUAL) {
            valveOpening = manualOpening;
            manualValveChanged = true;
        } else if (nextMode == SystemMode.AUTOMATIC) {
            valveOpening = valveState.openingPercentage();
        } else {
            setValveState(ValveState.CLOSED);
        }
    }

    public synchronized void setValveState(final ValveState nextState) {
        if (valveState == nextState) {
            return;
        }
        valveState = nextState;
        valveOpening = nextState.openingPercentage();
        valveStateChanged = true;
    }

    public synchronized void setManualOpening(final int opening, final boolean forwardToWcs) {
        final int bounded = clampPercentage(opening);
        if (manualOpening == bounded) {
            if (mode == SystemMode.MANUAL && forwardToWcs) {
                manualValveChanged = true;
            }
            return;
        }
        manualOpening = bounded;
        if (mode == SystemMode.MANUAL) {
            valveOpening = bounded;
            manualValveChanged = manualValveChanged || forwardToWcs;
        }
    }

    public synchronized void requestMode(final SystemMode nextMode) {
        requestedMode = nextMode;
    }

    public synchronized SystemMode consumeRequestedMode() {
        final SystemMode current = requestedMode;
        requestedMode = null;
        return current;
    }

    public synchronized void requestManualOpening(final int opening) {
        requestedManualOpening = clampPercentage(opening);
    }

    public synchronized Integer consumeRequestedManualOpening() {
        final Integer current = requestedManualOpening;
        requestedManualOpening = null;
        return current;
    }

    public synchronized boolean consumeModeChanged() {
        final boolean changed = modeChanged;
        modeChanged = false;
        return changed;
    }

    public synchronized boolean consumeValveStateChanged() {
        final boolean changed = valveStateChanged;
        valveStateChanged = false;
        return changed;
    }

    public synchronized boolean consumeManualValveChanged() {
        final boolean changed = manualValveChanged;
        manualValveChanged = false;
        return changed;
    }

    public synchronized void setLevelTimer(final boolean set, final long timestamp) {
        levelTimerSet = set;
        waterLevelTimestamp = timestamp;
    }

    public synchronized void setMqttConnected(final boolean connected) {
        mqttConnected = connected;
    }

    public synchronized void setWcsConnected(final boolean connected) {
        wcsConnected = connected;
    }

    public synchronized void setHttpAvailable(final boolean available) {
        httpAvailable = available;
    }

    private static int clampPercentage(final int value) {
        return Math.max(0, Math.min(100, value));
    }

    public record Snapshot(
        SystemMode mode,
        ValveState valveState,
        int valveOpening,
        int manualOpening,
        int waterLevel,
        boolean waterLevelAvailable,
        boolean levelTimerSet,
        boolean mqttConnected,
        boolean wcsConnected,
        boolean httpAvailable,
        long lastTmsMessage,
        long waterLevelTimestamp
    ) {
    }
}
