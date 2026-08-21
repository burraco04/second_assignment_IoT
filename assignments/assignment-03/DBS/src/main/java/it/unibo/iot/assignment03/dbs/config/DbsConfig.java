package it.unibo.iot.assignment03.dbs.config;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Properties;

public record DbsConfig(
    String cusBaseUrl,
    int pollPeriodMs,
    int historySize
) {
    public static DbsConfig load(final String fileName) throws IOException {
        final Properties properties = new Properties();
        final Path path = Path.of(fileName);
        if (Files.exists(path)) {
            try (InputStream input = Files.newInputStream(path)) {
                properties.load(input);
            }
        }

        return new DbsConfig(
            text(properties, "cus.baseUrl", "http://localhost:8080"),
            number(properties, "poll.period.ms", 1000),
            number(properties, "history.size", 60)
        );
    }

    private static String text(final Properties properties, final String key, final String defaultValue) {
        return properties.getProperty(key, defaultValue).trim().replaceAll("/+$", "");
    }

    private static int number(final Properties properties, final String key, final int defaultValue) {
        return Integer.parseInt(properties.getProperty(key, Integer.toString(defaultValue)).trim());
    }
}

