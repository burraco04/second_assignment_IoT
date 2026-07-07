#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include <Arduino.h>

class HangarTask: public Task {

public:
  HangarTask(Context* pContext);
  void tick();

private:
  void setState(Context::HangarState state);
  bool checkAndSetJustEntered();
  void log(const String& msg);
  void updateHangarState();

  Context* pContext;
  Context::HangarState state;
  bool justEntered;
  long preAlarmTimestamp;
  long alarmTimestamp;
};

#endif
