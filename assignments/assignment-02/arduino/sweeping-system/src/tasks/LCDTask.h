#ifndef __LCD_TASK__
#define __LCD_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class LCDTask: public Task {

public:
  LCDTask(Context* pContext);
  void tick();

private:
  enum State { RESTING_VIEW, TAKE_OFF_VIEW, FLYING_VIEW, LANDING_VIEW, ALARM_VIEW } state;

  void setState(State state);
  bool checkAndSetJustEntered();
  void log(const String& msg);
  State resolveState();
  void printCurrentState();

  Context* pContext;
  LiquidCrystal_I2C lcd;
  long stateTimestamp;
  bool justEntered;
};

#endif
