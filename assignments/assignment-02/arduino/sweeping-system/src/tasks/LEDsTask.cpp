#include "tasks/LEDsTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"

LEDsTask::LEDsTask(Led* l1, Led* l2, Led* l3, Context* pContext):
    l1(l1), l2(l2), l3(l3), pContext(pContext){
  setState(T0);
}

void LEDsTask::tick(){
  switch (state){
    case T0: {
      if (checkAndSetJustEntered()){
        log(F("[LED] T0"));
      }

      if (!l1->isOn()){
        l1->switchOn();
      }
      if (l2->isOn()){
        l2->switchOff();
      }
      if (l3->isOn()){
        l3->switchOff();
      }

      if (pContext->isSuspended()){
        setState(T2);
      } else if (pContext->isTakingOff() || pContext->isLanding()){
        setState(T1);
      }
      break;
    }

    case T1: {
      if (checkAndSetJustEntered()){
        log(F("[LED] T1"));
        l1->switchOff();
        l2->switchOn();
        l3->switchOff();
      } else {
        if (l2->isOn()){
          l2->switchOff();
        } else {
          l2->switchOn();
        }
      }

      if (pContext->isSuspended()){
        setState(T2);
      } else if (pContext->isResting() || pContext->isFlying()){
        setState(T0);
      }
      break;
    }

    case T2: {
      if (checkAndSetJustEntered()){
        log(F("[LED] T2"));
        l1->switchOff();
        l2->switchOff();
        l3->switchOn();
      }

      if (!pContext->isSuspended()){
        setState(T0);
      }
      break;
    }
  }
}

void LEDsTask::setState(int s){
  state = s;
  stateTimestamp = millis();
  justEntered = true;
}

long LEDsTask::elapsedTimeInState(){
  return millis() - stateTimestamp;
}

bool LEDsTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void LEDsTask::log(const String& msg){
  Logger.log(msg);
}
