#include <WiFi.h>
#include <PubSubClient.h>

#include "Scheduler.h"
#include "SonarSensor.h"
#include "config.h"

enum class NetworkState {
    Disconnected,
    WifiConnecting,
    MqttConnecting,
    Connected
};

enum class LedState {
    Error,
    Ok
};

enum class MeasuringState {
    Measuring
};

struct TmsContext {
    int waterLevel = 0;
    float distanceCm = NAN;
    bool networkConnected = false;
    bool lastPublishOk = false;
    bool connectionError = true;
    unsigned long sequence = 0;
    unsigned long lastSuccessfulPublish = 0;
};

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Scheduler scheduler;
SonarSensor sonar(SONAR_TRIG_PIN, SONAR_ECHO_PIN, SONAR_TIMEOUT_US);
TmsContext context;

static int computeWaterLevel(const float distanceCm) {
    if (isnan(distanceCm)) {
        return context.waterLevel;
    }

    const float normalized = (SONAR_EMPTY_CM - distanceCm) / (SONAR_EMPTY_CM - SONAR_FULL_CM);
    const int percentage = static_cast<int>(round(normalized * 100.0f));
    return constrain(percentage, 0, 100);
}

static bool mqttCredentialsConfigured() {
    return MQTT_USER[0] != '\0';
}

static void refreshConnectionError() {
    context.connectionError = !(context.networkConnected && context.lastPublishOk);
}

static void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // TMS currently publishes telemetry only. Callback is registered for future CUS commands.
    (void) topic;
    (void) payload;
    (void) length;
}

class NetworkTask : public Task {
public:
    NetworkTask() : Task(NETWORK_PERIOD_MS) {}

    void init() override {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        mqttClient.setServer(MQTT_HOST, MQTT_PORT);
        mqttClient.setCallback(mqttCallback);
    }

    void tick() override {
        const unsigned long now = millis();

        if (WiFi.status() != WL_CONNECTED) {
            handleWifiDisconnected(now);
            return;
        }

        if (!mqttClient.connected()) {
            handleMqttDisconnected(now);
            return;
        }

        mqttClient.loop();
        state = NetworkState::Connected;
        context.networkConnected = true;
        refreshConnectionError();
    }

private:
    NetworkState state = NetworkState::Disconnected;
    unsigned long lastWifiAttempt = 0;
    unsigned long lastMqttAttempt = 0;

    void handleWifiDisconnected(const unsigned long now) {
        context.networkConnected = false;
        refreshConnectionError();
        mqttClient.disconnect();

        if (now - lastWifiAttempt >= WIFI_RECONNECT_INTERVAL_MS || state == NetworkState::Disconnected) {
            lastWifiAttempt = now;
            WiFi.disconnect(false);
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            state = NetworkState::WifiConnecting;
        }
    }

    void handleMqttDisconnected(const unsigned long now) {
        context.networkConnected = false;
        refreshConnectionError();

        if (now - lastMqttAttempt < MQTT_RECONNECT_INTERVAL_MS && state == NetworkState::MqttConnecting) {
            return;
        }

        lastMqttAttempt = now;
        state = NetworkState::MqttConnecting;

        char clientId[40];
        snprintf(clientId, sizeof(clientId), "assignment03-tms-%llX", ESP.getEfuseMac());

        bool connected;
        if (mqttCredentialsConfigured()) {
            connected = mqttClient.connect(
                clientId,
                MQTT_USER,
                MQTT_PASSWORD,
                MQTT_TOPIC_AVAILABILITY,
                1,
                true,
                "offline"
            );
        } else {
            connected = mqttClient.connect(
                clientId,
                MQTT_TOPIC_AVAILABILITY,
                1,
                true,
                "offline"
            );
        }

        if (connected) {
            mqttClient.publish(MQTT_TOPIC_AVAILABILITY, "online", true);
            state = NetworkState::Connected;
            context.networkConnected = true;
            refreshConnectionError();
        }
    }
};

class MeasuringTask : public Task {
public:
    MeasuringTask() : Task(MEASURING_PERIOD_MS) {}

    void init() override {
        sonar.init();
    }

    void tick() override {
        switch (state) {
        case MeasuringState::Measuring:
            context.distanceCm = sonar.readDistanceCm();
            context.waterLevel = computeWaterLevel(context.distanceCm);
            publishTelemetry();
            break;
        }
    }

private:
    MeasuringState state = MeasuringState::Measuring;

    void publishTelemetry() {
        if (WiFi.status() != WL_CONNECTED || !mqttClient.connected()) {
            context.networkConnected = false;
            context.lastPublishOk = false;
            refreshConnectionError();
            return;
        }

        char payload[160];
        if (isnan(context.distanceCm)) {
            snprintf(
                payload,
                sizeof(payload),
                "{\"seq\":%lu,\"ts\":%lu,\"waterLevel\":%d,\"distanceCm\":null}",
                context.sequence++,
                millis(),
                context.waterLevel
            );
        } else {
            snprintf(
                payload,
                sizeof(payload),
                "{\"seq\":%lu,\"ts\":%lu,\"waterLevel\":%d,\"distanceCm\":%.2f}",
                context.sequence++,
                millis(),
                context.waterLevel,
                context.distanceCm
            );
        }

        const bool sent = mqttClient.publish(MQTT_TOPIC_TELEMETRY, payload);
        context.networkConnected = true;
        context.lastPublishOk = sent;
        refreshConnectionError();
        if (sent) {
            context.lastSuccessfulPublish = millis();
            mqttClient.publish(MQTT_TOPIC_STATUS, "ok", true);
        } else {
            mqttClient.publish(MQTT_TOPIC_STATUS, "error", true);
        }
    }
};

class LedTask : public Task {
public:
    LedTask() : Task(LED_PERIOD_MS) {}

    void init() override {
        pinMode(GREEN_LED_PIN, OUTPUT);
        pinMode(RED_LED_PIN, OUTPUT);
        applyOutputs();
    }

    void tick() override {
        if (context.connectionError && state != LedState::Error) {
            state = LedState::Error;
            applyOutputs();
        } else if (!context.connectionError && state != LedState::Ok) {
            state = LedState::Ok;
            applyOutputs();
        }
    }

private:
    LedState state = LedState::Error;

    void applyOutputs() const {
        digitalWrite(GREEN_LED_PIN, state == LedState::Ok ? HIGH : LOW);
        digitalWrite(RED_LED_PIN, state == LedState::Error ? HIGH : LOW);
    }
};

NetworkTask networkTask;
MeasuringTask measuringTask;
LedTask ledTask;

void setup() {
    Serial.begin(115200);

    scheduler.addTask(&networkTask);
    scheduler.addTask(&measuringTask);
    scheduler.addTask(&ledTask);
    scheduler.init();
}

void loop() {
    scheduler.schedule();
}
