#include "SerialTask.h"

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

SerialTask::SerialTask(WcsContext& context) : Task(SERIAL_PERIOD_MS), context(context) {}

void SerialTask::init() {
    Serial.println(F("HELLO WCS"));
}

void SerialTask::tick() {
    state = SerialState::Reading;
    readSerial();

    state = SerialState::Writing;
    sendPendingModeRequest();
    sendManualOpeningUpdate();
}

void SerialTask::readSerial() {
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

void SerialTask::handleLine(char* line) {
    if (strcmp(line, "OPEN") == 0) {
        context.automaticValveCommand = WcsValveCommand::Open;
    } else if (strcmp(line, "SEMI-OPEN") == 0) {
        context.automaticValveCommand = WcsValveCommand::SemiOpen;
    } else if (strcmp(line, "CLOSE") == 0) {
        context.automaticValveCommand = WcsValveCommand::Close;
    } else if (strncmp(line, "MODE ", 5) == 0) {
        handleModeCommand(line + 5);
    } else if (strncmp(line, "VALVE ", 6) == 0) {
        handleValveCommand(line + 6);
    } else if (strncmp(line, "OPENING ", 8) == 0) {
        handleValveCommand(line + 8);
    } else if (strcmp(line, "PING") == 0) {
        Serial.println(F("PONG"));
    }
}

void SerialTask::handleModeCommand(const char* value) {
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

void SerialTask::handleValveCommand(const char* value) {
    context.manualOpening = parseOpening(value);
    context.remoteManualOpening = true;
    context.lastPotRawValue = analogRead(POT_PIN);
}

void SerialTask::sendPendingModeRequest() {
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

void SerialTask::sendManualOpeningUpdate() {
    if (!context.manualOpeningChanged || context.mode != WcsMode::Manual) {
        return;
    }

    Serial.print(F("VALVE "));
    Serial.println(context.manualOpening);
    context.manualOpeningChanged = false;
}
