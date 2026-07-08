#ifndef __DDD_TASK__
#define __DDD_TASK__

#include "kernel/Task.h"
#include "devices/Sonar.h"
#include "model/Context.h"
#include <Arduino.h>

class DDDTask: public Task {

public:
  DDDTask(Sonar* pSonar, Context* pContext);
  void tick();

private:
  enum State { SLEEPING, MEASURING } state;

  void setState(State state);
  bool checkAndSetJustEntered();
  void log(const String& msg);
  void updateDistance();
  void updateDronePhase();
  bool isDroneOut(float distance);
  bool isDroneLanded(float distance);

  Sonar* pSonar;
  Context* pContext;
  long stateTimestamp;
  long conditionTimestamp;
  bool justEntered;
};

#endif
