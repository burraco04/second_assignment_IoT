#pragma once

#include <LiquidCrystal.h>

#include "../../Scheduler.h"
#include "WcsContext.h"

enum class LcdState {
    Display
};

class LcdTask : public Task {
public:
    LcdTask(WcsContext& context, LiquidCrystal& lcd);

    void init() override;
    void tick() override;

private:
    WcsContext& context;
    LiquidCrystal& lcd;
    LcdState state = LcdState::Display;

    void printPadded(uint8_t row, const char* text);
};

