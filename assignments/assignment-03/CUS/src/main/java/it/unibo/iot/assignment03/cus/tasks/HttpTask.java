package it.unibo.iot.assignment03.cus.tasks;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpServer;
import it.unibo.iot.assignment03.cus.config.CusConfig;
import it.unibo.iot.assignment03.cus.model.CusContext;
import it.unibo.iot.assignment03.cus.model.SystemMode;
import it.unibo.iot.assignment03.cus.scheduler.Task;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.Executors;
import java.util.regex.Pattern;

public final class HttpTask implements Task {
    private static final Pattern OPENING_PATTERN = Pattern.compile("\"(?:valve|opening)\"\\s*:\\s*(\\d+)");

    private final CusConfig config;
    private final CusContext context;
    private final HttpClient httpClient = HttpClient.newHttpClient();
    private HttpServer server;

    public HttpTask(final CusConfig config, final CusContext context) {
        this.config = config;
        this.context = context;
    }

    @Override
    public String name() {
        return "HTTP task";
    }

    @Override
    public long periodMillis() {
        return config.httpPeriodMs();
    }

    @Override
    public void init() throws IOException {
        server = HttpServer.create(new InetSocketAddress(config.httpPort()), 0);
        server.createContext("/api/state", this::handleState);
        server.createContext("/api/mode", this::handleMode);
        server.createContext("/api/valve", this::handleValve);
        server.setExecutor(Executors.newCachedThreadPool());
        server.start();
        context.setHttpAvailable(true);
    }

    @Override
    public void tick() throws Exception {
        context.setHttpAvailable(true);
        if (!config.dbsPushUrl().isBlank()) {
            final var request = HttpRequest.newBuilder(URI.create(config.dbsPushUrl()))
                .header("Content-Type", "application/json")
                .POST(HttpRequest.BodyPublishers.ofString(snapshotJson()))
                .build();
            httpClient.send(request, HttpResponse.BodyHandlers.discarding());
        }
    }

    private void handleState(final HttpExchange exchange) throws IOException {
        if (handleOptions(exchange)) {
            return;
        }
        if (!exchange.getRequestMethod().equals("GET")) {
            respond(exchange, 405, "{\"error\":\"method not allowed\"}");
            return;
        }
        respond(exchange, 200, snapshotJson());
    }

    private void handleMode(final HttpExchange exchange) throws IOException {
        if (handleOptions(exchange)) {
            return;
        }
        if (!exchange.getRequestMethod().equals("POST")) {
            respond(exchange, 405, "{\"error\":\"method not allowed\"}");
            return;
        }
        final String body = requestBody(exchange).toUpperCase();
        if (body.contains("MANUAL")) {
            context.requestMode(SystemMode.MANUAL);
        } else if (body.contains("AUTOMATIC") || body.contains("AUTO")) {
            context.requestMode(SystemMode.AUTOMATIC);
        }
        respond(exchange, 202, snapshotJson());
    }

    private void handleValve(final HttpExchange exchange) throws IOException {
        if (handleOptions(exchange)) {
            return;
        }
        if (!exchange.getRequestMethod().equals("POST")) {
            respond(exchange, 405, "{\"error\":\"method not allowed\"}");
            return;
        }
        final var matcher = OPENING_PATTERN.matcher(requestBody(exchange));
        if (matcher.find()) {
            context.requestManualOpening(Integer.parseInt(matcher.group(1)));
        }
        respond(exchange, 202, snapshotJson());
    }

    private String snapshotJson() {
        final var snapshot = context.snapshot();
        return "{"
            + "\"mode\":\"" + snapshot.mode() + "\","
            + "\"waterLevel\":" + snapshot.waterLevel() + ","
            + "\"waterLevelAvailable\":" + snapshot.waterLevelAvailable() + ","
            + "\"valveOpening\":" + snapshot.valveOpening() + ","
            + "\"valveState\":\"" + snapshot.valveState().displayName() + "\","
            + "\"mqttConnected\":" + snapshot.mqttConnected() + ","
            + "\"wcsConnected\":" + snapshot.wcsConnected() + ","
            + "\"httpAvailable\":" + snapshot.httpAvailable()
            + "}";
    }

    private static String requestBody(final HttpExchange exchange) throws IOException {
        return new String(exchange.getRequestBody().readAllBytes(), StandardCharsets.UTF_8);
    }

    private static boolean handleOptions(final HttpExchange exchange) throws IOException {
        if (!exchange.getRequestMethod().equals("OPTIONS")) {
            return false;
        }
        respond(exchange, 204, "");
        return true;
    }

    private static void respond(final HttpExchange exchange, final int status, final String body) throws IOException {
        final byte[] bytes = body.getBytes(StandardCharsets.UTF_8);
        exchange.getResponseHeaders().add("Content-Type", "application/json");
        exchange.getResponseHeaders().add("Access-Control-Allow-Origin", "*");
        exchange.getResponseHeaders().add("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        exchange.getResponseHeaders().add("Access-Control-Allow-Headers", "Content-Type");
        exchange.sendResponseHeaders(status, bytes.length);
        exchange.getResponseBody().write(bytes);
        exchange.close();
    }
}
