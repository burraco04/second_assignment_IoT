#pragma once

#include <Arduino.h>

struct TmsContext {
    int waterLevel = 0;
    float distanceCm = NAN;
    bool networkConnected = false;
    bool lastPublishOk = false;
    bool connectionError = true;
    unsigned long sequence = 0;
    unsigned long lastSuccessfulPublish = 0;

    void refreshConnectionError() {
        connectionError = !(networkConnected && lastPublishOk);
    }
};

