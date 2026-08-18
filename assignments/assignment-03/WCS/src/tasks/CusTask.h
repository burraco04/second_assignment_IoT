#pragma once

#include "../../Scheduler.h"
#include "WcsContext.h"

enum class CusState {
    Automatic,
    Manual,
    Unconnected
};

class CusTask : public Task {
public:
    explicit CusTask(WcsContext& context);

    void init() override;
    void tick() override;

private:
    static constexpr uint8_t RX_BUFFER_SIZE = 64;

    WcsContext& context;
    CusState state = CusState::Automatic;
    char rxBuffer[RX_BUFFER_SIZE] = {};
    uint8_t rxLength = 0;
    unsigned long lastModeRequestSent = 0;

    void readSerial();
    void handleLine(char* line);
    void handleModeCommand(const char* value);
    void handleValveCommand(const char* value);
    void handleStateCommand(char* value);
    void handleTimeout();
    void sendPendingRequests();
    void sendManualOpeningUpdate();
    void restoreConnectedState();
    void updateStateFromContext();
};

