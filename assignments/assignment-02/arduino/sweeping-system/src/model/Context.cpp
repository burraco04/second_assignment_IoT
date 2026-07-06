#include "Context.h"

Context::Context(){
  started = false;
  stopped = false;
  droneState = RESTING;
  resumeState = RESTING;
}

bool Context::isStarted(){
  return started;
}

bool Context::isStopped(){
  return stopped;
}

void Context::setStarted(){
  started = true;
  stopped = false;
}

void Context::setStopped(){
  stopped = true;
  started = false;
}

void Context::reset(){
  started = false;
  stopped = false;
  droneState = RESTING;
  resumeState = RESTING;
}

void Context::setResting(){
  droneState = RESTING;
  resumeState = RESTING;
}

void Context::setTakingOff(){
  droneState = TAKING_OFF;
}

void Context::setFlying(){
  droneState = FLYING;
}

void Context::setLanding(){
  droneState = LANDING;
}

void Context::setSuspended(){
  if (droneState != SUSPENDED){
    resumeState = droneState;
  }
  droneState = SUSPENDED;
}

void Context::clearSuspended(){
  if (droneState == SUSPENDED){
    droneState = resumeState;
  }
}

Context::DroneState Context::getDroneState(){
  return droneState;
}

bool Context::isResting(){
  return droneState == RESTING;
}

bool Context::isTakingOff(){
  return droneState == TAKING_OFF;
}

bool Context::isFlying(){
  return droneState == FLYING;
}

bool Context::isLanding(){
  return droneState == LANDING;
}

bool Context::isSuspended(){
  return droneState == SUSPENDED;
}

