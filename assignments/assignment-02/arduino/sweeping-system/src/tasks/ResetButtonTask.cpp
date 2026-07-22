#include "tasks/ResetButtonTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

ResetButtonTask::ResetButtonTask(Button* pButton, Context* pContext):
    pButton(pButton), pContext(pContext){
  wasPressed = false;
  setState(WAITING);
}

void ResetButtonTask::tick(){
  switch (state){
    case WAITING: {
      if (checkAndSetJustEntered()){
        log(F("[RESET] WAITING"));
      }

      bool pressed = pButton->isPressed();
      if (pressed && !wasPressed && resetEnabled()){
        setState(RESETTING);
      }
      wasPressed = pressed;
      break;
    }

    case RESETTING: {
      if (checkAndSetJustEntered()){
        log(F("[RESET] RESETTING"));
        if (pContext->resetAlarm()){
          MsgService.sendMsg(String(F("HANGAR:")) + hangarStateToString(pContext->getHangarState()));
          MsgService.sendMsg(String(F("STATE:")) + droneStateToString(pContext->getDroneState()));
        }
      }

      if (!pButton->isPressed()){
        wasPressed = false;
        setState(WAITING);
      }
      break;
    }
  }
}

void ResetButtonTask::setState(State s){
  state = s;
  stateTimestamp = millis();
  justEntered = true;
}

bool ResetButtonTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void ResetButtonTask::log(const String& msg){
  Logger.log(msg);
}

bool ResetButtonTask::resetEnabled(){
  return pContext->isAlarm();
}

String ResetButtonTask::droneStateToString(Context::DroneState s){
  switch (s){
    case Context::RESTING: return "RESTING";
    case Context::TAKING_OFF: return "TAKING_OFF";
    case Context::FLYING: return "FLYING";
    case Context::LANDING: return "LANDING";
    case Context::SUSPENDED: return "SUSPENDED";
  }
  return "UNKNOWN";
}

String ResetButtonTask::hangarStateToString(Context::HangarState s){
  switch (s){
    case Context::NORMAL: return "NORMAL";
    case Context::PRE_ALARM: return "PRE_ALARM";
    case Context::ALARM: return "ALARM";
  }
  return "UNKNOWN";
}
