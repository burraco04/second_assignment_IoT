#pragma once

#include <Arduino.h>

#include "../../config.h"

enum class WcsMode {
    Automatic,
    Manual,
    Unconnected
};

inline const char* modeToText(const WcsMode mode) {
    switch (mode) {
    case WcsMode::Automatic:
        return "AUTOMATIC";
    case WcsMode::Manual:
        return "MANUAL";
    case WcsMode::Unconnected:
        return "UNCONNECTED";
    }
    return "UNKNOWN";
}

struct WcsContext {
    WcsMode mode = WcsMode::Automatic;
    WcsMode lastConnectedMode = WcsMode::Automatic;
    WcsMode requestedMode = WcsMode::Automatic;

    int cusOpening = 0;
    int manualOpening = 0;
    int valveOpening = 0;

    bool modeChangeRequested = false;
    bool manualOpeningChanged = false;

    unsigned long lastCusMessage = 0;

    bool isConnected() const {
        return mode != WcsMode::Unconnected;
    }

    bool isManual() const {
        return mode == WcsMode::Manual;
    }

    void setConnectedMode(const WcsMode nextMode) {
        mode = nextMode;
        lastConnectedMode = nextMode;
    }

    int desiredOpening() const {
        if (mode == WcsMode::Unconnected) {
            return MIN_OPENING_PERCENT;
        }
        return mode == WcsMode::Manual ? manualOpening : cusOpening;
    }
};

