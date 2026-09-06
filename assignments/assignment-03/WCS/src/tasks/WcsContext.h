#pragma once

#include <Arduino.h>

#include "../../config.h"

enum class WcsMode {
    Automatic,
    Manual,
    Unconnected
};

enum class WcsValveCommand {
    Close,
    SemiOpen,
    Open
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

inline int openingForCommand(const WcsValveCommand command) {
    switch (command) {
    case WcsValveCommand::Close:
        return 0;
    case WcsValveCommand::SemiOpen:
        return 50;
    case WcsValveCommand::Open:
        return 100;
    }
    return 0;
}

struct WcsContext {
    WcsMode mode = WcsMode::Automatic;
    WcsMode lastConnectedMode = WcsMode::Automatic;
    WcsMode requestedMode = WcsMode::Automatic;
    WcsValveCommand automaticValveCommand = WcsValveCommand::Close;

    int manualOpening = 0;
    int valveOpening = 0;
    int lastPotRawValue = -1;

    bool modeChangeRequested = false;
    bool manualOpeningChanged = false;
    bool remoteManualOpening = false;

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
        return mode == WcsMode::Manual ? manualOpening : openingForCommand(automaticValveCommand);
    }
};
