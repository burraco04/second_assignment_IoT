#ifndef __HANGAR_DOOR_TASK__
#define __HANGAR_DOOR_TASK__

#include "kernel/Task.h"
#include "devices/ServoMotor.h"
#include "model/Context.h"
#include <Arduino.h>

class HangarDoorTask: public Task {

public:
  HangarDoorTask(ServoMotor* pMotor, Context* pContext);
  void tick();

private:
  enum State { CLOSED, OPENING, OPEN, CLOSING } state;

  void setState(State state);
  long elapsedTimeInState();
  bool checkAndSetJustEntered();
  void log(const String& msg);
  void ensureMotorOn();

  ServoMotor* pMotor;
  Context* pContext;
  long stateTimestamp;
  bool justEntered;
};

#endif
