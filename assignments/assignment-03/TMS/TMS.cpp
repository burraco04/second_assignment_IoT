#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "SonarSensor.h"
#include "config.h"
#include "Scheduler.h"
#include "src/tasks/LedTask.h"
#include "src/tasks/MeasuringTask.h"
#include "src/tasks/NetworkTask.h"
#include "src/tasks/TmsContext.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

Scheduler scheduler;
SonarSensor sonar(SONAR_TRIG_PIN, SONAR_ECHO_PIN, SONAR_TIMEOUT_US);
TmsContext context;

NetworkTask networkTask(mqttClient, context);
MeasuringTask measuringTask(mqttClient, sonar, context);
LedTask ledTask(context);

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
