#include "tasks/HangarDoorTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"

#define DOOR_CLOSED_POS 0
#define DOOR_OPEN_POS 90
#define DOOR_MOVE_TIME 1000

HangarDoorTask::HangarDoorTask(ServoMotor* pMotor, Context* pContext):
    pMotor(pMotor), pContext(pContext){
  setState(CLOSED);
}

void HangarDoorTask::tick(){
  switch (state){
    case CLOSED: {
      if (checkAndSetJustEntered()){
        log(F("[HD] CLOSED"));
        ensureMotorOn();
        pMotor->setPosition(DOOR_CLOSED_POS);
        pMotor->off();
      }

      if (!pContext->isAlarm() && (pContext->isTakingOff() || pContext->isLanding())){
        setState(OPENING);
      }
      break;
    }

    case OPENING: {
      if (checkAndSetJustEntered()){
        log(F("[HD] OPENING"));
        ensureMotorOn();
        pMotor->setPosition(DOOR_OPEN_POS);
      }

      if (pContext->isAlarm()){
        setState(CLOSING);
      } else if (elapsedTimeInState() >= DOOR_MOVE_TIME){
        setState(OPEN);
      }
      break;
    }

    case OPEN: {
      if (checkAndSetJustEntered()){
        log(F("[HD] OPEN"));
        pMotor->off();
      }

      if (pContext->isAlarm() || pContext->isFlying() || pContext->isResting()){
        setState(CLOSING);
      }
      break;
    }

    case CLOSING: {
      if (checkAndSetJustEntered()){
        log(F("[HD] CLOSING"));
        ensureMotorOn();
        pMotor->setPosition(DOOR_CLOSED_POS);
      }

      if (elapsedTimeInState() >= DOOR_MOVE_TIME){
        setState(CLOSED);
      }
      break;
    }
  }
}

void HangarDoorTask::setState(State s){
  state = s;
  stateTimestamp = millis();
  justEntered = true;
}

long HangarDoorTask::elapsedTimeInState(){
  return millis() - stateTimestamp;
}

bool HangarDoorTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void HangarDoorTask::log(const String& msg){
  Logger.log(msg);
}

void HangarDoorTask::ensureMotorOn(){
  if (!pMotor->isOn()){
    pMotor->on();
  }
}
