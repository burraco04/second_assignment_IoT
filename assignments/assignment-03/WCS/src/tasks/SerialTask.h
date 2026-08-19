#pragma once

#include "Scheduler.h"
#include "WcsContext.h"

enum class SerialState {
    Reading,
    Writing
};

class SerialTask : public Task {
public:
    explicit SerialTask(WcsContext& context);

    void init() override;
    void tick() override;

private:
    static constexpr uint8_t RX_BUFFER_SIZE = 64;

    WcsContext& context;
    SerialState state = SerialState::Reading;
    char rxBuffer[RX_BUFFER_SIZE] = {};
    uint8_t rxLength = 0;
    unsigned long lastModeRequestSent = 0;

    void readSerial();
    void handleLine(char* line);
    void handleModeCommand(const char* value);
    void handleValveCommand(const char* value);
    void sendPendingModeRequest();
    void sendManualOpeningUpdate();
};

