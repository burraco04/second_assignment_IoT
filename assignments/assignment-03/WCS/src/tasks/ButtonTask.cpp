#include "ButtonTask.h"

#include <Arduino.h>

ButtonTask::ButtonTask(WcsContext& context) : Task(BUTTON_PERIOD_MS), context(context) {}

void ButtonTask::init() {
    pinMode(BUTTON_PIN, BUTTON_ACTIVE_LOW ? INPUT_PULLUP : INPUT);
    lastRawPressed = readPressed();
    stablePressed = lastRawPressed;
}

void ButtonTask::tick() {
    const bool rawPressed = readPressed();
    const unsigned long now = millis();

    if (rawPressed != lastRawPressed) {
        lastRawPressed = rawPressed;
        lastChangeTime = now;
    }

    if (now - lastChangeTime < BUTTON_DEBOUNCE_MS || rawPressed == stablePressed) {
        return;
    }

    stablePressed = rawPressed;
    if (!stablePressed) {
        return;
    }

    state = ButtonState::ChangeMode;
    requestModeToggle();
    state = ButtonState::Waiting;
}

bool ButtonTask::readPressed() const {
    const int rawValue = digitalRead(BUTTON_PIN);
    return BUTTON_ACTIVE_LOW ? rawValue == LOW : rawValue == HIGH;
}

void ButtonTask::requestModeToggle() {
    const WcsMode currentMode = context.mode == WcsMode::Unconnected
        ? context.lastConnectedMode
        : context.mode;

    context.requestedMode = currentMode == WcsMode::Manual
        ? WcsMode::Automatic
        : WcsMode::Manual;
    context.modeChangeRequested = true;

    if (context.mode != WcsMode::Unconnected) {
        context.setConnectedMode(context.requestedMode);
    }
}

