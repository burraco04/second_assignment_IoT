package it.unibo.iot.assignment03.cus.config;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Properties;

public record CusConfig(
    String mqttBroker,
    String mqttClientId,
    String mqttUsername,
    String mqttPassword,
    String mqttTmsTopic,
    String serialPort,
    int serialBaudRate,
    int httpPort,
    String dbsPushUrl,
    int levelL1,
    int levelL2,
    long levelT1Ms,
    long tmsTimeoutMs,
    long cusPeriodMs,
    long mqttPeriodMs,
    long serialPeriodMs,
    long valvePeriodMs,
    long httpPeriodMs
) {
    public static CusConfig load(final String fileName) throws IOException {
        final var properties = new Properties();
        final Path path = Path.of(fileName);
        if (Files.exists(path)) {
            try (InputStream input = Files.newInputStream(path)) {
                properties.load(input);
            }
        }

        return new CusConfig(
            text(properties, "mqtt.broker", "tcp://broker.mqtt-dashboard.com:1883"),
            text(properties, "mqtt.clientId", "assignment03-cus"),
            text(properties, "mqtt.username", ""),
            text(properties, "mqtt.password", ""),
            text(properties, "mqtt.topic.tms", "assignment03/tms/telemetry"),
            text(properties, "serial.port", "COM3"),
            number(properties, "serial.baudRate", 9600),
            number(properties, "http.port", 8080),
            text(properties, "dbs.pushUrl", ""),
            number(properties, "level.l1", 50),
            number(properties, "level.l2", 80),
            number(properties, "level.t1.ms", 5000),
            number(properties, "tms.timeout.ms", 3000),
            number(properties, "task.cus.period.ms", 500),
            number(properties, "task.mqtt.period.ms", 500),
            number(properties, "task.serial.period.ms", 500),
            number(properties, "task.valve.period.ms", 500),
            number(properties, "task.http.period.ms", 500)
        );
    }

    private static String text(final Properties properties, final String key, final String defaultValue) {
        return properties.getProperty(key, defaultValue).trim();
    }

    private static int number(final Properties properties, final String key, final int defaultValue) {
        return Integer.parseInt(properties.getProperty(key, Integer.toString(defaultValue)).trim());
    }
}
