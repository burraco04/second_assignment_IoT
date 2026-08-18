#include "MeasuringTask.h"

#include <Arduino.h>
#include <WiFi.h>

#include "../../config.h"

MeasuringTask::MeasuringTask(PubSubClient& mqttClient, SonarSensor& sonar, TmsContext& context)
    : Task(MEASURING_PERIOD_MS), mqttClient(mqttClient), sonar(sonar), context(context) {}

void MeasuringTask::init() {
    sonar.init();
}

void MeasuringTask::tick() {
    switch (state) {
    case MeasuringState::Measuring:
        context.distanceCm = sonar.readDistanceCm();
        context.waterLevel = computeWaterLevel(context.distanceCm);
        publishTelemetry();
        break;
    }
}

int MeasuringTask::computeWaterLevel(const float distanceCm) const {
    if (isnan(distanceCm)) {
        return context.waterLevel;
    }

    const float normalized = (SONAR_EMPTY_CM - distanceCm) / (SONAR_EMPTY_CM - SONAR_FULL_CM);
    const int percentage = static_cast<int>(round(normalized * 100.0f));
    return constrain(percentage, 0, 100);
}

void MeasuringTask::publishTelemetry() {
    if (WiFi.status() != WL_CONNECTED || !mqttClient.connected()) {
        context.networkConnected = false;
        context.lastPublishOk = false;
        context.refreshConnectionError();
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
    context.refreshConnectionError();
    if (sent) {
        context.lastSuccessfulPublish = millis();
        mqttClient.publish(MQTT_TOPIC_STATUS, "ok", true);
    } else {
        mqttClient.publish(MQTT_TOPIC_STATUS, "error", true);
    }
}

