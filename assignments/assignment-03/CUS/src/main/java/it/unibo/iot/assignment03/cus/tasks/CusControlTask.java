package it.unibo.iot.assignment03.cus.tasks;

import it.unibo.iot.assignment03.cus.config.CusConfig;
import it.unibo.iot.assignment03.cus.model.CusContext;
import it.unibo.iot.assignment03.cus.model.SystemMode;
import it.unibo.iot.assignment03.cus.model.ValveState;
import it.unibo.iot.assignment03.cus.scheduler.Task;

public final class CusControlTask implements Task {
    private final CusConfig config;
    private final CusContext context;

    public CusControlTask(final CusConfig config, final CusContext context) {
        this.config = config;
        this.context = context;
    }

    @Override
    public String name() {
        return "CUS task";
    }

    @Override
    public long periodMillis() {
        return config.cusPeriodMs();
    }

    @Override
    public void tick() {
        final long now = System.currentTimeMillis();
        final var snapshot = context.snapshot();
        if (!snapshot.waterLevelAvailable() || now - snapshot.lastTmsMessage() > config.tmsTimeoutMs()) {
            context.setMode(SystemMode.UNCONNECTED);
            context.setValveState(ValveState.CLOSED);
            return;
        }

        if (snapshot.mode() == SystemMode.UNCONNECTED) {
            context.setMode(SystemMode.AUTOMATIC);
        }

        final SystemMode requestedMode = context.consumeRequestedMode();
        if (requestedMode != null) {
            context.setMode(requestedMode);
        }

        final Integer requestedOpening = context.consumeRequestedManualOpening();
        if (requestedOpening != null) {
            context.setManualOpening(requestedOpening, true);
        }
    }
}

