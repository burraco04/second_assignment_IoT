#include "tasks/DPDTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"

DPDTask::DPDTask(Pir* pPir, Context* pContext):
    pPir(pPir), pContext(pContext){
  setState(SLEEPING);
}

void DPDTask::tick(){
  switch (state){
    case SLEEPING: {
      if (checkAndSetJustEntered()){
        log(F("[DPD] SLEEPING"));
      }

      if (!(pContext->isFlying() || pContext->isLanding())){
        pContext->setDroneDetected(false);
      }

      if (!pContext->isAlarm() && (pContext->isFlying() || pContext->isLanding())){
        setState(DETECTING);
      }
      break;
    }

    case DETECTING: {
      if (checkAndSetJustEntered()){
        log(F("[DPD] DETECTING"));
      }

      pPir->sync();
      pContext->setDroneDetected(pPir->isDetected());
      setState(SLEEPING);
      break;
    }
  }
}

void DPDTask::setState(State s){
  state = s;
  stateTimestamp = millis();
  justEntered = true;
}

bool DPDTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void DPDTask::log(const String& msg){
  Logger.log(msg);
}
