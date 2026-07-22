#ifndef __RESET_BUTTON_TASK__
#define __RESET_BUTTON_TASK__

#include "kernel/Task.h"
#include "devices/Button.h"
#include "model/Context.h"
#include <Arduino.h>

class ResetButtonTask: public Task {

public:
  ResetButtonTask(Button* pButton, Context* pContext);
  void tick();

private:
  enum State { WAITING, RESETTING } state;

  void setState(State state);
  bool checkAndSetJustEntered();
  void log(const String& msg);
  bool resetEnabled();
  String droneStateToString(Context::DroneState state);
  String hangarStateToString(Context::HangarState state);

  Button* pButton;
  Context* pContext;
  long stateTimestamp;
  bool justEntered;
  bool wasPressed;
};

#endif
