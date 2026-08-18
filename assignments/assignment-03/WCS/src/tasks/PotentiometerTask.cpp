#include "PotentiometerTask.h"

#include <Arduino.h>

PotentiometerTask::PotentiometerTask(WcsContext& context)
    : Task(POTENTIOMETER_PERIOD_MS), context(context) {}

void PotentiometerTask::init() {
    pinMode(POT_PIN, INPUT);
}

void PotentiometerTask::tick() {
    if (context.mode != WcsMode::Manual) {
        state = PotentiometerState::Sleeping;
        return;
    }

    state = PotentiometerState::Measuring;

    const int rawValue = analogRead(POT_PIN);
    const int nextOpening = map(rawValue, 0, 1023, MIN_OPENING_PERCENT, MAX_OPENING_PERCENT);

    if (nextOpening != context.manualOpening) {
        context.manualOpening = nextOpening;
        context.manualOpeningChanged = true;
    }
}

