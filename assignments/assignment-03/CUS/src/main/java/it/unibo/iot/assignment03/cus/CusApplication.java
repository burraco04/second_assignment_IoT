package it.unibo.iot.assignment03.cus;

import it.unibo.iot.assignment03.cus.config.CusConfig;
import it.unibo.iot.assignment03.cus.model.CusContext;
import it.unibo.iot.assignment03.cus.scheduler.Scheduler;
import it.unibo.iot.assignment03.cus.tasks.CusControlTask;
import it.unibo.iot.assignment03.cus.tasks.HttpTask;
import it.unibo.iot.assignment03.cus.tasks.MqttTask;
import it.unibo.iot.assignment03.cus.tasks.SerialTask;
import it.unibo.iot.assignment03.cus.tasks.ValveRegulationTask;

public final class CusApplication {
    private CusApplication() {
    }

    public static void main(final String[] args) throws Exception {
        final var config = CusConfig.load(args.length > 0 ? args[0] : "config.properties");
        final var context = new CusContext();
        final var scheduler = new Scheduler();

        scheduler.addTask(new MqttTask(config, context));
        scheduler.addTask(new CusControlTask(config, context));
        scheduler.addTask(new ValveRegulationTask(config, context));
        scheduler.addTask(new SerialTask(config, context));
        scheduler.addTask(new HttpTask(config, context));

        Runtime.getRuntime().addShutdownHook(new Thread(scheduler::stop));
        scheduler.start();
    }
}

