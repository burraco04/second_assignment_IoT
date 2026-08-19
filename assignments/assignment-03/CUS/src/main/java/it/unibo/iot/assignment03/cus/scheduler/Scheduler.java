package it.unibo.iot.assignment03.cus.scheduler;

import java.util.ArrayList;
import java.util.List;

public final class Scheduler {
    private final List<ScheduledTask> tasks = new ArrayList<>();
    private volatile boolean running;

    public void addTask(final Task task) {
        tasks.add(new ScheduledTask(task));
    }

    public void start() throws Exception {
        for (final ScheduledTask task : tasks) {
            task.task().init();
        }

        running = true;
        while (running) {
            final long now = System.currentTimeMillis();
            for (final ScheduledTask task : tasks) {
                if (now - task.lastActivation() >= task.task().periodMillis()) {
                    task.setLastActivation(now);
                    try {
                        task.task().tick();
                    } catch (final Exception ex) {
                        System.err.println("Task " + task.task().name() + " failed: " + ex.getMessage());
                    }
                }
            }
            Thread.sleep(10);
        }
    }

    public void stop() {
        running = false;
    }

    private static final class ScheduledTask {
        private final Task task;
        private long lastActivation;

        private ScheduledTask(final Task task) {
            this.task = task;
        }

        private Task task() {
            return task;
        }

        private long lastActivation() {
            return lastActivation;
        }

        private void setLastActivation(final long lastActivation) {
            this.lastActivation = lastActivation;
        }
    }
}

