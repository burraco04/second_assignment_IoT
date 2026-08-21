package it.unibo.iot.assignment03.dbs.http;

import it.unibo.iot.assignment03.dbs.config.DbsConfig;
import it.unibo.iot.assignment03.dbs.model.CusState;
import it.unibo.iot.assignment03.dbs.model.SystemMode;
import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;
import java.util.regex.Pattern;

public final class CusHttpClient {
    private static final Duration TIMEOUT = Duration.ofSeconds(2);
    private static final Pattern STRING_FIELD = Pattern.compile("\"([^\"]+)\"\\s*:\\s*\"([^\"]*)\"");
    private static final Pattern NUMBER_FIELD = Pattern.compile("\"([^\"]+)\"\\s*:\\s*(\\d+)");
    private static final Pattern BOOLEAN_FIELD = Pattern.compile("\"([^\"]+)\"\\s*:\\s*(true|false)");

    private final String baseUrl;
    private final HttpClient client;

    public CusHttpClient(final DbsConfig config) {
        this.baseUrl = config.cusBaseUrl();
        this.client = HttpClient.newBuilder().connectTimeout(TIMEOUT).build();
    }

    public CusState readState() throws IOException, InterruptedException {
        final HttpRequest request = HttpRequest.newBuilder(URI.create(baseUrl + "/api/state"))
            .timeout(TIMEOUT)
            .GET()
            .build();
        final HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
        if (response.statusCode() < 200 || response.statusCode() >= 300) {
            throw new IOException("CUS returned HTTP " + response.statusCode());
        }
        return parseState(response.body());
    }

    public void setMode(final SystemMode mode) throws IOException, InterruptedException {
        postJson("/api/mode", "{\"mode\":\"" + mode.name() + "\"}");
    }

    public void setValveOpening(final int opening) throws IOException, InterruptedException {
        postJson("/api/valve", "{\"valve\":" + clamp(opening) + "}");
    }

    private void postJson(final String path, final String json) throws IOException, InterruptedException {
        final HttpRequest request = HttpRequest.newBuilder(URI.create(baseUrl + path))
            .timeout(TIMEOUT)
            .header("Content-Type", "application/json")
            .POST(HttpRequest.BodyPublishers.ofString(json))
            .build();
        final HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
        if (response.statusCode() < 200 || response.statusCode() >= 300) {
            throw new IOException("CUS returned HTTP " + response.statusCode());
        }
    }

    private static CusState parseState(final String json) {
        final String modeText = stringField(json, "mode", "NOT_AVAILABLE");
        return new CusState(
            parseMode(modeText),
            numberField(json, "waterLevel", 0),
            booleanField(json, "waterLevelAvailable", false),
            numberField(json, "valveOpening", 0),
            stringField(json, "valveState", "UNKNOWN"),
            booleanField(json, "mqttConnected", false),
            booleanField(json, "wcsConnected", false),
            booleanField(json, "httpAvailable", false)
        );
    }

    private static String stringField(final String json, final String field, final String defaultValue) {
        final var matcher = STRING_FIELD.matcher(json);
        while (matcher.find()) {
            if (matcher.group(1).equals(field)) {
                return matcher.group(2);
            }
        }
        return defaultValue;
    }

    private static int numberField(final String json, final String field, final int defaultValue) {
        final var matcher = NUMBER_FIELD.matcher(json);
        while (matcher.find()) {
            if (matcher.group(1).equals(field)) {
                return clamp(Integer.parseInt(matcher.group(2)));
            }
        }
        return defaultValue;
    }

    private static boolean booleanField(final String json, final String field, final boolean defaultValue) {
        final var matcher = BOOLEAN_FIELD.matcher(json);
        while (matcher.find()) {
            if (matcher.group(1).equals(field)) {
                return Boolean.parseBoolean(matcher.group(2));
            }
        }
        return defaultValue;
    }

    private static SystemMode parseMode(final String value) {
        return switch (value) {
            case "AUTOMATIC" -> SystemMode.AUTOMATIC;
            case "MANUAL" -> SystemMode.MANUAL;
            case "UNCONNECTED" -> SystemMode.UNCONNECTED;
            default -> SystemMode.NOT_AVAILABLE;
        };
    }

    private static int clamp(final int value) {
        return Math.max(0, Math.min(100, value));
    }
}

