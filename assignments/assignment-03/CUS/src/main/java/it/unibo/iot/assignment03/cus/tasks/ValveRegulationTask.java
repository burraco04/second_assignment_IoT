package it.unibo.iot.assignment03.cus.tasks;

import it.unibo.iot.assignment03.cus.config.CusConfig;
import it.unibo.iot.assignment03.cus.model.CusContext;
import it.unibo.iot.assignment03.cus.model.SystemMode;
import it.unibo.iot.assignment03.cus.model.ValveState;
import it.unibo.iot.assignment03.cus.scheduler.Task;

public final class ValveRegulationTask implements Task {
    private final CusConfig config;
    private final CusContext context;

    public ValveRegulationTask(final CusConfig config, final CusContext context) {
        this.config = config;
        this.context = context;
    }

    @Override
    public String name() {
        return "Valve regulation task";
    }

    @Override
    public long periodMillis() {
        return config.valvePeriodMs();
    }

    @Override
    public void tick() {
        final var snapshot = context.snapshot();
        if (snapshot.mode() != SystemMode.AUTOMATIC || !snapshot.waterLevelAvailable()) {
            return;
        }

        final int level = snapshot.waterLevel();
        final long now = System.currentTimeMillis();

        if (level > config.levelL2()) {
            context.setLevelTimer(false, snapshot.waterLevelTimestamp());
            context.setValveState(ValveState.OPEN);
            return;
        }

        switch (snapshot.valveState()) {
            case CLOSED -> {
                if (level < config.levelL1()) {
                    context.setLevelTimer(false, snapshot.waterLevelTimestamp());
                } else if (level > config.levelL1() && level < config.levelL2() && !snapshot.levelTimerSet()) {
                    context.setLevelTimer(true, now);
                } else if (snapshot.levelTimerSet() && now - snapshot.waterLevelTimestamp() > config.levelT1Ms()) {
                    context.setValveState(ValveState.SEMI_OPEN);
                }
            }
            case SEMI_OPEN -> {
                if (level < config.levelL1()) {
                    context.setLevelTimer(false, snapshot.waterLevelTimestamp());
                    context.setValveState(ValveState.CLOSED);
                } else if (level > config.levelL2()) {
                    context.setValveState(ValveState.OPEN);
                }
            }
            case OPEN -> {
                if (level < config.levelL2()) {
                    context.setValveState(ValveState.SEMI_OPEN);
                }
            }
        }
    }
}

