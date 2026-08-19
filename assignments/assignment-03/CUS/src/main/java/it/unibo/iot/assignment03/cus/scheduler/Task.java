package it.unibo.iot.assignment03.cus.scheduler;

public interface Task {
    String name();

    long periodMillis();

    default void init() throws Exception {
    }

    void tick() throws Exception;
}

