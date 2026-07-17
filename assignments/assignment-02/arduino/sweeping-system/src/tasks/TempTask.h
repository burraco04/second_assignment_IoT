#ifndef __TEMP_TASK__
#define __TEMP_TASK__

#include "kernel/Task.h"
#include "devices/TempSensor.h"
#include "model/Context.h"
#include <Arduino.h>

class TempTask: public Task {

public:
  TempTask(TempSensor* pTempSensor, Context* pContext);
  void tick();

private:
  enum State { MEASURING, SLEEPING } state;

  void setState(State state);
  bool checkAndSetJustEntered();
  void log(const String& msg);

  TempSensor* pTempSensor;
  Context* pContext;

  long stateTimestamp;
  bool justEntered;
};

#endif
