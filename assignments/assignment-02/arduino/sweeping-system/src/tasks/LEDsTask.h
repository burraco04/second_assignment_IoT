#ifndef __LEDS_TASK__
#define __LEDS_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include "devices/Led.h"
#include <Arduino.h>

class LEDsTask: public Task {

public:
  LEDsTask(Led* l1, Led* l2, Led* l3, Context* pContext); 
  void tick();

private:  
  void setState(int state);
  long elapsedTimeInState();
  void log(const String& msg);
  
  bool checkAndSetJustEntered();
  
  enum State { T0, T1, T2 } state;
  long stateTimestamp;
  bool justEntered;

  Led* l1;
  Led* l2;
  Led* l3;
  Context* pContext;
};

#endif
