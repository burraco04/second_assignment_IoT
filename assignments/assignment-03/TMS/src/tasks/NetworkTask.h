#pragma once

#include <PubSubClient.h>

#include "../../Scheduler.h"
#include "TmsContext.h"

enum class NetworkState {
    Disconnected,
    WifiConnecting,
    MqttConnecting,
    Connected
};

class NetworkTask : public Task {
public:
    NetworkTask(PubSubClient& mqttClient, TmsContext& context);

    void init() override;
    void tick() override;

private:
    PubSubClient& mqttClient;
    TmsContext& context;
    NetworkState state = NetworkState::Disconnected;
    unsigned long lastWifiAttempt = 0;
    unsigned long lastMqttAttempt = 0;

    void handleWifiDisconnected(unsigned long now);
    void handleMqttDisconnected(unsigned long now);
};

