#include "tasks/TempTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"

TempTask::TempTask(TempSensor* pTempSensor, Context* pContext):
    pTempSensor(pTempSensor), pContext(pContext){
  setState(MEASURING);
}

void TempTask::tick(){
  switch (state){
    case MEASURING: {
      if (checkAndSetJustEntered()){
        log(F("[TEMP] MEASURING"));
      }

      float temp = pTempSensor->getTemperature();
      pContext->setTemperature(temp);

      if (pContext->isSuspended()){
        setState(SLEEPING);
      }
      break;
    }

    case SLEEPING: {
      if (checkAndSetJustEntered()){
        log(F("[TEMP] SLEEPING"));
      }

      if (!pContext->isSuspended()){
        setState(MEASURING);
      }
      break;
    }
  }
}

void TempTask::setState(State s){
  state = s;
  stateTimestamp = millis();
  justEntered = true;
}

bool TempTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void TempTask::log(const String& msg){
  Logger.log(msg);
}
