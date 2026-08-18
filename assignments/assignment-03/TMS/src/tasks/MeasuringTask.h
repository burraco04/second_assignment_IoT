#pragma once

#include <PubSubClient.h>

#include "../../Scheduler.h"
#include "../../SonarSensor.h"
#include "TmsContext.h"

enum class MeasuringState {
    Measuring
};

class MeasuringTask : public Task {
public:
    MeasuringTask(PubSubClient& mqttClient, SonarSensor& sonar, TmsContext& context);

    void init() override;
    void tick() override;

private:
    PubSubClient& mqttClient;
    SonarSensor& sonar;
    TmsContext& context;
    MeasuringState state = MeasuringState::Measuring;

    int computeWaterLevel(float distanceCm) const;
    void publishTelemetry();
};

