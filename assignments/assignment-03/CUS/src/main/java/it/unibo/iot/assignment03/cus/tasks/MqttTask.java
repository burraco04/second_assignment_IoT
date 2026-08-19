package it.unibo.iot.assignment03.cus.tasks;

import it.unibo.iot.assignment03.cus.config.CusConfig;
import it.unibo.iot.assignment03.cus.model.CusContext;
import it.unibo.iot.assignment03.cus.scheduler.Task;
import java.nio.charset.StandardCharsets;
import java.util.regex.Pattern;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public final class MqttTask implements Task {
    private static final Pattern WATER_LEVEL_PATTERN = Pattern.compile("\"waterLevel\"\\s*:\\s*(\\d+)");

    private final CusConfig config;
    private final CusContext context;
    private MqttClient client;

    public MqttTask(final CusConfig config, final CusContext context) {
        this.config = config;
        this.context = context;
    }

    @Override
    public String name() {
        return "MQTT task";
    }

    @Override
    public long periodMillis() {
        return config.mqttPeriodMs();
    }

    @Override
    public void init() throws Exception {
        client = new MqttClient(config.mqttBroker(), config.mqttClientId(), new MemoryPersistence());
        client.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(final Throwable cause) {
                context.setMqttConnected(false);
            }

            @Override
            public void messageArrived(final String topic, final MqttMessage message) {
                final String payload = new String(message.getPayload(), StandardCharsets.UTF_8);
                final var matcher = WATER_LEVEL_PATTERN.matcher(payload);
                if (matcher.find()) {
                    context.onWaterLevel(Integer.parseInt(matcher.group(1)), System.currentTimeMillis());
                }
            }

            @Override
            public void deliveryComplete(final org.eclipse.paho.client.mqttv3.IMqttDeliveryToken token) {
            }
        });
    }

    @Override
    public void tick() throws Exception {
        if (client.isConnected()) {
            context.setMqttConnected(true);
            return;
        }

        final var options = new MqttConnectOptions();
        options.setAutomaticReconnect(false);
        options.setCleanSession(true);
        if (!config.mqttUsername().isBlank()) {
            options.setUserName(config.mqttUsername());
            options.setPassword(config.mqttPassword().toCharArray());
        }

        client.connect(options);
        client.subscribe(config.mqttTmsTopic());
        context.setMqttConnected(true);
    }
}

