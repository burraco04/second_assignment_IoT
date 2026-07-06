#ifndef __DRONE_TASK__
#define __DRONE_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include <Arduino.h>

class DroneTask: public Task {

public:
  DroneTask(Context* pContext);
  void tick();

private:
  enum { WAITING, APPLY_RESTING, APPLY_TAKING_OFF, APPLY_FLYING, APPLY_LANDING, APPLY_SUSPENDED, APPLY_NORMAL } state;

  void setState(int state);
  bool checkAndSetJustEntered();
  void log(const String& msg);
  String stateToString(Context::DroneState state);
  void applyCommand(const String& command);

  long stateTimestamp;
  bool justEntered;
  Context* pContext;
};

#endif
