#include "LedTask.h"

#include <Arduino.h>

#include "../../config.h"

LedTask::LedTask(TmsContext& context) : Task(LED_PERIOD_MS), context(context) {}

void LedTask::init() {
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    applyOutputs();
}

void LedTask::tick() {
    if (context.connectionError && state != LedState::Error) {
        state = LedState::Error;
        applyOutputs();
    } else if (!context.connectionError && state != LedState::Ok) {
        state = LedState::Ok;
        applyOutputs();
    }
}

void LedTask::applyOutputs() const {
    digitalWrite(GREEN_LED_PIN, state == LedState::Ok ? HIGH : LOW);
    digitalWrite(RED_LED_PIN, state == LedState::Error ? HIGH : LOW);
}

