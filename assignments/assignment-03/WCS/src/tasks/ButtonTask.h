#pragma once

#include "Scheduler.h"
#include "WcsContext.h"

enum class ButtonState {
    Waiting,
    ChangeMode
};

class ButtonTask : public Task {
public:
    explicit ButtonTask(WcsContext& context);

    void init() override;
    void tick() override;

private:
    WcsContext& context;
    ButtonState state = ButtonState::Waiting;
    bool lastRawPressed = false;
    bool stablePressed = false;
    unsigned long lastChangeTime = 0;

    bool readPressed() const;
    void requestModeToggle();
};
