#pragma once

#include "Scheduler.h"
#include "TmsContext.h"

enum class LedState {
    Error,
    Ok
};

class LedTask : public Task {
public:
    explicit LedTask(TmsContext& context);

    void init() override;
    void tick() override;

private:
    TmsContext& context;
    LedState state = LedState::Error;

    void applyOutputs() const;
};
