#include "ValveTask.h"

#include <Arduino.h>

ValveTask::ValveTask(WcsContext& context, Servo& servo)
    : Task(VALVE_PERIOD_MS), context(context), servo(servo) {}

void ValveTask::init() {
    servo.attach(SERVO_PIN);
    applyOpening(MIN_OPENING_PERCENT);
}

void ValveTask::tick() {
    const int opening = constrain(context.desiredOpening(), MIN_OPENING_PERCENT, MAX_OPENING_PERCENT);
    updateState(opening);
    applyOpening(opening);
}

void ValveTask::applyOpening(const int opening) {
    const int angle = map(opening, MIN_OPENING_PERCENT, MAX_OPENING_PERCENT, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
    servo.write(angle);
    context.valveOpening = opening;
}

void ValveTask::updateState(const int opening) {
    if (opening <= MIN_OPENING_PERCENT) {
        state = ValveState::Closed;
    } else if (opening >= MAX_OPENING_PERCENT) {
        state = ValveState::Open;
    } else if (context.mode == WcsMode::Manual) {
        state = ValveState::ManualPosition;
    } else {
        state = ValveState::SemiOpen;
    }
}

