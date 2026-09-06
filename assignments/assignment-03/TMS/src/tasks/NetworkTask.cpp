#include "NetworkTask.h"

#include <Arduino.h>
#include <WiFi.h>

#include "../../config.h"

static bool mqttCredentialsConfigured() {
    return MQTT_USER[0] != '\0';
}

static void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // TMS currently publishes telemetry only. Callback is registered for future CUS commands.
    (void) topic;
    (void) payload;
    (void) length;
}

NetworkTask::NetworkTask(PubSubClient& mqttClient, TmsContext& context)
    : Task(NETWORK_PERIOD_MS), mqttClient(mqttClient), context(context) {}

void NetworkTask::init() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
}

void NetworkTask::tick() {
    const unsigned long now = millis();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WIFI DISCONNECTED");
        handleWifiDisconnected(now);
        return;
    }

    if (!mqttClient.connected()) {
        Serial.println("MQTT DISCONNECTED");
        handleMqttDisconnected(now);
        return;
    }

    mqttClient.loop();
    state = NetworkState::Connected;
    context.networkConnected = true;
    context.refreshConnectionError();
}

void NetworkTask::handleWifiDisconnected(const unsigned long now) {
    context.networkConnected = false;
    context.refreshConnectionError();
    mqttClient.disconnect();

    if (now - lastWifiAttempt >= WIFI_RECONNECT_INTERVAL_MS || state == NetworkState::Disconnected) {
        lastWifiAttempt = now;
        WiFi.disconnect(false);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        state = NetworkState::WifiConnecting;
    }
}

void NetworkTask::handleMqttDisconnected(const unsigned long now) {
    context.networkConnected = false;
    context.refreshConnectionError();

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
        context.refreshConnectionError();
    } else {
        Serial.printf("MQTT connect failed, state=%d, host=%s, port=%u\n", mqttClient.state(), MQTT_HOST, MQTT_PORT);
    }
}

