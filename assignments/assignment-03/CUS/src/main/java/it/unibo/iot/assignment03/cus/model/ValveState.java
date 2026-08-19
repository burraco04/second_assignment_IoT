package it.unibo.iot.assignment03.cus.model;

public enum ValveState {
    CLOSED,
    SEMI_OPEN,
    OPEN;

    public int openingPercentage() {
        return switch (this) {
            case CLOSED -> 0;
            case SEMI_OPEN -> 50;
            case OPEN -> 100;
        };
    }

    public String serialCommand() {
        return switch (this) {
            case CLOSED -> "CLOSE";
            case SEMI_OPEN -> "SEMI-OPEN";
            case OPEN -> "OPEN";
        };
    }

    public String displayName() {
        return switch (this) {
            case CLOSED -> "CLOSED";
            case SEMI_OPEN -> "SEMI-OPEN";
            case OPEN -> "OPEN";
        };
    }
}

