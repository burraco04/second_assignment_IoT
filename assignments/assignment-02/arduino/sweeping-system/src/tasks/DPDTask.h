#ifndef __DPD_TASK__
#define __DPD_TASK__

#include "kernel/Task.h"
#include "devices/Pir.h"
#include "model/Context.h"
#include <Arduino.h>

class DPDTask: public Task {

public:
  DPDTask(Pir* pPir, Context* pContext);
  void tick();

private:
  enum State { SLEEPING, DETECTING } state;

  void setState(State state);
  bool checkAndSetJustEntered();
  void log(const String& msg);

  Pir* pPir;
  Context* pContext;
  long stateTimestamp;
  bool justEntered;
};

#endif
