#include "tasks/DDDTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

#define D1 0.50
#define D2 0.10
#define T1 3000
#define T2 3000

DDDTask::DDDTask(Sonar* pSonar, Context* pContext):
    pSonar(pSonar), pContext(pContext){
  conditionTimestamp = 0;
  setState(SLEEPING);
}

void DDDTask::tick(){
  switch (state){
    case SLEEPING: {
      if (checkAndSetJustEntered()){
        log(F("[DDD] SLEEPING"));
        conditionTimestamp = 0;
      }

      if (!pContext->isAlarm() && (pContext->isTakingOff() || pContext->isLanding())){
        setState(MEASURING);
      }
      break;
    }

    case MEASURING: {
      if (checkAndSetJustEntered()){
        log(F("[DDD] MEASURING"));
      }

      updateDistance();
      updateDronePhase();

      if (pContext->isAlarm() || pContext->isResting() || pContext->isFlying()){
        setState(SLEEPING);
      }
      break;
    }
  }
}

void DDDTask::updateDistance(){
  pSonar->setTemperature(pContext->getTemperature());
  float distance = pSonar->getDistance();
  pContext->setDistance(distance);

  if (pContext->isLanding()){
    MsgService.sendMsg(String(F("DISTANCE:")) + String(distance, 2));
  }
}

void DDDTask::updateDronePhase(){
  float distance = pContext->getDistance();
  long now = millis();
  bool conditionMatched = false;

  if (pContext->isTakingOff()){
    conditionMatched = isDroneOut(distance);
    if (conditionMatched && conditionTimestamp != 0 && (now - conditionTimestamp) >= T1){
      pContext->setFlying();
      MsgService.sendMsg(String(F("STATE:FLYING")));
      conditionTimestamp = 0;
      return;
    }
  } else if (pContext->isLanding()){
    conditionMatched = isDroneLanded(distance);
    if (conditionMatched && conditionTimestamp != 0 && (now - conditionTimestamp) >= T2){
      pContext->setResting();
      MsgService.sendMsg(String(F("STATE:RESTING")));
      conditionTimestamp = 0;
      return;
    }
  }

  if (conditionMatched){
    if (conditionTimestamp == 0){
      conditionTimestamp = now;
    }
  } else {
    conditionTimestamp = 0;
  }
}

bool DDDTask::isDroneOut(float distance){
  return distance == NO_OBJ_DETECTED || distance > D1;
}

bool DDDTask::isDroneLanded(float distance){
  return distance != NO_OBJ_DETECTED && distance < D2;
}

void DDDTask::setState(int s){
  state = s;
  stateTimestamp = millis();
  justEntered = true;
}

bool DDDTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void DDDTask::log(const String& msg){
  Logger.log(msg);
}
