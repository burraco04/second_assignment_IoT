#pragma once

#include <LiquidCrystal_I2C.h>

#include "WcsContext.h"
#include "Scheduler.h"

enum class LcdState {
    Display
};

class LcdTask : public Task {
public:
    LcdTask(WcsContext& context, LiquidCrystal_I2C& lcd);

    void init() override;
    void tick() override;

private:
    WcsContext& context;
    LiquidCrystal_I2C& lcd;
    LcdState state = LcdState::Display;

    void printPadded(uint8_t row, const char* text);
};
