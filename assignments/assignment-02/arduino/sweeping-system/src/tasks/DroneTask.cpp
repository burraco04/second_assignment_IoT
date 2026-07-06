#include "tasks/DroneTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

DroneTask::DroneTask(Context* pContext): pContext(pContext){
  setState(WAITING);
}

void DroneTask::tick(){
  switch (state){
    case WAITING: {
      if (checkAndSetJustEntered()){
        log(F("[DRONE] WAITING"));
      }

      if (MsgService.isMsgAvailable()){
        Msg* msg = MsgService.receiveMsg();
        if (msg != NULL){
          applyCommand(msg->getContent());
          delete msg;
        }
      }
      break;
    }

    case APPLY_RESTING: {
      if (checkAndSetJustEntered()){
        pContext->setResting();
        log(F("[DRONE] RESTING"));
        MsgService.sendMsg(String(F("STATE:")) + stateToString(pContext->getDroneState()));
        setState(WAITING);
      }
      break;
    }

    case APPLY_TAKING_OFF: {
      if (checkAndSetJustEntered()){
        pContext->setTakingOff();
        log(F("[DRONE] TAKING_OFF"));
        MsgService.sendMsg(String(F("STATE:")) + stateToString(pContext->getDroneState()));
        setState(WAITING);
      }
      break;
    }

    case APPLY_FLYING: {
      if (checkAndSetJustEntered()){
        pContext->setFlying();
        log(F("[DRONE] FLYING"));
        MsgService.sendMsg(String(F("STATE:")) + stateToString(pContext->getDroneState()));
        setState(WAITING);
      }
      break;
    }

    case APPLY_LANDING: {
      if (checkAndSetJustEntered()){
        pContext->setLanding();
        log(F("[DRONE] LANDING"));
        MsgService.sendMsg(String(F("STATE:")) + stateToString(pContext->getDroneState()));
        setState(WAITING);
      }
      break;
    }

    case APPLY_SUSPENDED: {
      if (checkAndSetJustEntered()){
        pContext->setSuspended();
        log(F("[DRONE] SUSPENDED"));
        MsgService.sendMsg(String(F("STATE:")) + stateToString(pContext->getDroneState()));
        setState(WAITING);
      }
      break;
    }

    case APPLY_NORMAL: {
      if (checkAndSetJustEntered()){
        pContext->clearSuspended();
        log(F("[DRONE] NORMAL"));
        MsgService.sendMsg(String(F("STATE:")) + stateToString(pContext->getDroneState()));
        setState(WAITING);
      }
      break;
    }
  }
}

void DroneTask::applyCommand(const String& command){
  String cmd = command;
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "REST" || cmd == "DRONE:REST"){
    setState(APPLY_RESTING);
  } else if (cmd == "TAKEOFF" || cmd == "DRONE:TAKEOFF" || cmd == "TAKE_OFF"){
    setState(APPLY_TAKING_OFF);
  } else if (cmd == "FLY" || cmd == "DRONE:FLY"){
    setState(APPLY_FLYING);
  } else if (cmd == "LAND" || cmd == "DRONE:LAND"){
    setState(APPLY_LANDING);
  } else if (cmd == "SUSPEND" || cmd == "SYSTEM:SUSPEND" || cmd == "ALARM"){
    setState(APPLY_SUSPENDED);
  } else if (cmd == "NORMAL" || cmd == "SYSTEM:NORMAL" || cmd == "RESET"){
    setState(APPLY_NORMAL);
  } else {
    log(String(F("[DRONE] IGNORED ")) + cmd);
  }
}

void DroneTask::setState(int s){
  state = s;
  stateTimestamp = millis();
  justEntered = true;
}

bool DroneTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void DroneTask::log(const String& msg){
  Logger.log(msg);
}

String DroneTask::stateToString(Context::DroneState s){
  switch (s){
    case Context::RESTING: return "RESTING";
    case Context::TAKING_OFF: return "TAKING_OFF";
    case Context::FLYING: return "FLYING";
    case Context::LANDING: return "LANDING";
    case Context::SUSPENDED: return "SUSPENDED";
  }
  return "UNKNOWN";
}
