#include "CusTask.h"

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

static bool parseMode(const char* value, WcsMode& mode) {
    if (strcmp(value, "AUTOMATIC") == 0 || strcmp(value, "AUTO") == 0) {
        mode = WcsMode::Automatic;
        return true;
    }
    if (strcmp(value, "MANUAL") == 0) {
        mode = WcsMode::Manual;
        return true;
    }
    if (strcmp(value, "UNCONNECTED") == 0) {
        mode = WcsMode::Unconnected;
        return true;
    }
    return false;
}

static int parseOpening(const char* value) {
    return constrain(atoi(value), MIN_OPENING_PERCENT, MAX_OPENING_PERCENT);
}

CusTask::CusTask(WcsContext& context) : Task(CUS_PERIOD_MS), context(context) {}

void CusTask::init() {
    context.lastCusMessage = millis();
    Serial.println(F("HELLO WCS"));
}

void CusTask::tick() {
    readSerial();
    handleTimeout();
    sendPendingRequests();
    sendManualOpeningUpdate();
    updateStateFromContext();
}

void CusTask::readSerial() {
    while (Serial.available() > 0) {
        const char next = static_cast<char>(Serial.read());

        if (next == '\r') {
            continue;
        }

        if (next == '\n') {
            rxBuffer[rxLength] = '\0';
            if (rxLength > 0) {
                handleLine(rxBuffer);
            }
            rxLength = 0;
            continue;
        }

        if (rxLength < RX_BUFFER_SIZE - 1) {
            rxBuffer[rxLength++] = next;
        } else {
            rxLength = 0;
        }
    }
}

void CusTask::handleLine(char* line) {
    context.lastCusMessage = millis();
    restoreConnectedState();

    if (strncmp(line, "MODE ", 5) == 0) {
        handleModeCommand(line + 5);
    } else if (strncmp(line, "VALVE ", 6) == 0) {
        handleValveCommand(line + 6);
    } else if (strncmp(line, "OPENING ", 8) == 0) {
        handleValveCommand(line + 8);
    } else if (strncmp(line, "STATE ", 6) == 0) {
        handleStateCommand(line + 6);
    } else if (strcmp(line, "PING") == 0) {
        Serial.println(F("PONG"));
    }
}

void CusTask::handleModeCommand(const char* value) {
    WcsMode nextMode;
    if (!parseMode(value, nextMode)) {
        return;
    }

    if (nextMode == WcsMode::Unconnected) {
        context.mode = WcsMode::Unconnected;
    } else {
        context.setConnectedMode(nextMode);
    }

    if (context.modeChangeRequested && context.requestedMode == nextMode) {
        context.modeChangeRequested = false;
    }
}

void CusTask::handleValveCommand(const char* value) {
    context.cusOpening = parseOpening(value);
}

void CusTask::handleStateCommand(char* value) {
    char* opening = strchr(value, ' ');
    if (opening != nullptr) {
        *opening = '\0';
        handleValveCommand(opening + 1);
    }
    handleModeCommand(value);
}

void CusTask::handleTimeout() {
    if (millis() - context.lastCusMessage <= CUS_TIMEOUT_MS) {
        return;
    }

    context.mode = WcsMode::Unconnected;
}

void CusTask::sendPendingRequests() {
    if (!context.modeChangeRequested) {
        return;
    }

    const unsigned long now = millis();
    if (now - lastModeRequestSent < MODE_REQUEST_RESEND_MS) {
        return;
    }

    Serial.print(F("MODE "));
    Serial.println(modeToText(context.requestedMode));
    lastModeRequestSent = now;
}

void CusTask::sendManualOpeningUpdate() {
    if (!context.manualOpeningChanged || context.mode != WcsMode::Manual) {
        return;
    }

    Serial.print(F("VALVE "));
    Serial.println(context.manualOpening);
    context.manualOpeningChanged = false;
}

void CusTask::restoreConnectedState() {
    if (context.mode != WcsMode::Unconnected) {
        return;
    }
    context.mode = context.lastConnectedMode;
}

void CusTask::updateStateFromContext() {
    switch (context.mode) {
    case WcsMode::Automatic:
        state = CusState::Automatic;
        break;
    case WcsMode::Manual:
        state = CusState::Manual;
        break;
    case WcsMode::Unconnected:
        state = CusState::Unconnected;
        break;
    }
}

