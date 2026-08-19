#pragma once

#include <Servo.h>

#include "WcsContext.h"
#include "Scheduler.h"

enum class ValveState {
    Closed,
    SemiOpen,
    Open,
    ManualPosition
};

class ValveTask : public Task {
public:
    ValveTask(WcsContext& context, Servo& servo);

    void init() override;
    void tick() override;

private:
    WcsContext& context;
    Servo& servo;
    ValveState state = ValveState::Closed;

    void applyOpening(int opening);
    void updateState(int opening);
};
