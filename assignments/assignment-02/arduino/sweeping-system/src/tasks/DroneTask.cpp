#include "tasks/DroneTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

#define D1 0.50
#define D2 0.10
#define T1 3000
#define T2 3000

DroneTask::DroneTask(Context* pContext): pContext(pContext){
  state = RESTING;
  stateTimestamp = millis();
  conditionTimestamp = 0;
  justEntered = true;
  takeOffSignal = false;
  landingSignal = false;
  pContext->setResting();
}

void DroneTask::tick(){
  readCommand();
  syncStateFromContext();

  if (checkAndSetJustEntered()){
    log(String(F("[DRONE] ")) + stateToString(state));
  }

  switch (state){
    case RESTING:
      updateResting();
      break;

    case TAKING_OFF:
      updateTakingOff();
      break;

    case FLYING:
      updateFlying();
      break;

    case LANDING:
      updateLanding();
      break;

    case SUSPENDED:
      updateSuspended();
      break;
  }
}

void DroneTask::updateResting(){
  resetConditionTimer();
  landingSignal = false;

  if (pContext->isAlarm()){
    setState(SUSPENDED);
  } else if (takeOffSignal){
    takeOffSignal = false;
    if (pContext->isPreAlarm()){
      log(F("[DRONE] TAKEOFF_BLOCKED"));
    } else {
      setState(TAKING_OFF);
    }
  }
}

void DroneTask::updateTakingOff(){
  takeOffSignal = false;
  landingSignal = false;

  if (pContext->isAlarm()){
    setState(SUSPENDED);
  } else if (isDroneOut()){
    if (conditionHeldFor(T1)){
      setState(FLYING);
    }
  } else {
    resetConditionTimer();
  }
}

void DroneTask::updateFlying(){
  resetConditionTimer();
  takeOffSignal = false;

  if (pContext->isAlarm()){
    landingSignal = false;
    setState(SUSPENDED);
  } else if (landingSignal){
    landingSignal = false;
    if (pContext->isPreAlarm()){
      log(F("[DRONE] LAND_BLOCKED"));
    } else if (!pContext->isDroneDetected()){
      log(F("[DRONE] LAND_WAITING_DPD"));
    } else {
      setState(LANDING);
    }
  }
}

void DroneTask::updateLanding(){
  takeOffSignal = false;
  landingSignal = false;

  if (pContext->isAlarm()){
    setState(SUSPENDED);
  } else if (isDroneLanded()){
    if (conditionHeldFor(T2)){
      setState(RESTING);
    }
  } else {
    resetConditionTimer();
  }
}

void DroneTask::updateSuspended(){
  takeOffSignal = false;
  landingSignal = false;
  resetConditionTimer();

  if (pContext->isAlarm()){
    return;
  }

  if (isDroneOutsideAfterSuspension()){
    setState(FLYING);
  } else if (isDroneLanded()){
    setState(RESTING);
  }
}

void DroneTask::readCommand(){
  if (!MsgService.isMsgAvailable()){
    return;
  }

  Msg* msg = MsgService.receiveMsg();
  if (msg != NULL){
    applyCommand(msg->getContent());
    delete msg;
  }
}

void DroneTask::applyCommand(const String& command){
  String cmd = command;
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "TAKEOFF" || cmd == "DRONE:TAKEOFF" || cmd == "TAKE_OFF"){
    takeOffSignal = true;
  } else if (cmd == "LAND" || cmd == "DRONE:LAND"){
    landingSignal = true;
  } else if (cmd == "ALARM" || cmd == "SYSTEM:ALARM"){
    pContext->setAlarm();
  } else if (cmd == "SUSPEND" || cmd == "SYSTEM:SUSPEND"){
    pContext->setSuspended();
  } else if (cmd == "RESET" || cmd == "SYSTEM:RESET"){
    takeOffSignal = false;
    landingSignal = false;
    resetConditionTimer();
    pContext->reset();
  } else if (cmd == "REST" || cmd == "DRONE:REST"){
    takeOffSignal = false;
    landingSignal = false;
    resetConditionTimer();
    pContext->setResting();
  } else if (cmd == "FLY" || cmd == "DRONE:FLY"){
    takeOffSignal = false;
    landingSignal = false;
    resetConditionTimer();
    pContext->setFlying();
  } else if (cmd == "NORMAL" || cmd == "SYSTEM:NORMAL"){
    if (pContext->isAlarm()){
      log(F("[DRONE] NORMAL_BLOCKED"));
    } else {
      pContext->clearSuspended();
    }
  } else {
    log(String(F("[DRONE] IGNORED ")) + cmd);
  }
}

void DroneTask::syncStateFromContext(){
  State contextState = stateFromContext(pContext->getDroneState());
  if (contextState != state){
    state = contextState;
    stateTimestamp = millis();
    conditionTimestamp = 0;
    justEntered = true;
  }
}

DroneTask::State DroneTask::stateFromContext(Context::DroneState s){
  switch (s){
    case Context::RESTING:
      return RESTING;
    case Context::TAKING_OFF:
      return TAKING_OFF;
    case Context::FLYING:
      return FLYING;
    case Context::LANDING:
      return LANDING;
    case Context::SUSPENDED:
      return SUSPENDED;
  }
  return RESTING;
}

void DroneTask::setState(State s){
  if (state == s){
    return;
  }

  state = s;
  stateTimestamp = millis();
  conditionTimestamp = 0;
  justEntered = true;
  updateContextState(s);
  notifyState();
}

void DroneTask::updateContextState(State s){
  switch (s){
    case RESTING:
      pContext->setResting();
      break;
    case TAKING_OFF:
      pContext->setTakingOff();
      break;
    case FLYING:
      pContext->setFlying();
      break;
    case LANDING:
      pContext->setLanding();
      break;
    case SUSPENDED:
      pContext->setSuspended();
      break;
  }
}

void DroneTask::notifyState(){
  MsgService.sendMsg(String(F("STATE:")) + stateToString(state));
}

void DroneTask::resetConditionTimer(){
  conditionTimestamp = 0;
}

bool DroneTask::conditionHeldFor(long duration){
  long now = millis();

  if (conditionTimestamp == 0){
    conditionTimestamp = now;
    return false;
  }

  return (now - conditionTimestamp) >= duration;
}

bool DroneTask::isDroneOut(){
  float distance = pContext->getDistance();
  return distance < 0 || distance > D1;
}

bool DroneTask::isDroneLanded(){
  float distance = pContext->getDistance();
  return distance >= 0 && distance < D2;
}

bool DroneTask::isDroneOutsideAfterSuspension(){
  float distance = pContext->getDistance();
  return distance < 0 || distance > D2;
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

String DroneTask::stateToString(State s){
  switch (s){
    case RESTING:
      return "RESTING";
    case TAKING_OFF:
      return "TAKING_OFF";
    case FLYING:
      return "FLYING";
    case LANDING:
      return "LANDING";
    case SUSPENDED:
      return "SUSPENDED";
  }
  return "UNKNOWN";
}
