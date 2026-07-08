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
        pContext->reset();
        MsgService.sendMsg(String(F("HANGAR:NORMAL")));
        MsgService.sendMsg(String(F("STATE:RESTING")));
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
  return pContext->isAlarm() || pContext->isPreAlarm() || pContext->isSuspended();
}
