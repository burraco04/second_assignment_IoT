#pragma once

#include "../../Scheduler.h"
#include "WcsContext.h"

enum class PotentiometerState {
    Sleeping,
    Measuring
};

class PotentiometerTask : public Task {
public:
    explicit PotentiometerTask(WcsContext& context);

    void init() override;
    void tick() override;

private:
    WcsContext& context;
    PotentiometerState state = PotentiometerState::Sleeping;
};

