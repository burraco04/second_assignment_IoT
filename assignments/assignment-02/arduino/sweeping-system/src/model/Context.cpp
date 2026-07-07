#include "Context.h"

Context::Context(){
  started = false;
  stopped = false;
  droneDetected = false;
  droneState = RESTING;
  resumeState = RESTING;
  hangarState = NORMAL;
  temperature = 0.0;
  distance = 0.0;
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
  droneDetected = false;
  droneState = RESTING;
  resumeState = RESTING;
  hangarState = NORMAL;
  temperature = 0.0;
  distance = 0.0;
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

void Context::setHangarNormal(){
  hangarState = NORMAL;
}

void Context::setHangarPreAlarm(){
  hangarState = PRE_ALARM;
}

void Context::setHangarAlarm(){
  hangarState = ALARM;
}

void Context::setAlarm(){
  setHangarAlarm();
}

void Context::clearAlarm(){
  hangarState = NORMAL;
}

Context::DroneState Context::getDroneState(){
  return droneState;
}

Context::HangarState Context::getHangarState(){
  return hangarState;
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

bool Context::isNormal(){
  return hangarState == NORMAL;
}

bool Context::isPreAlarm(){
  return hangarState == PRE_ALARM;
}

bool Context::isAlarm(){
  return hangarState == ALARM;
}

void Context::setTemperature(float temp){
  temperature = temp;
}

float Context::getTemperature(){
  return temperature;
}

void Context::setDistance(float distance){
  this->distance = distance;
}

float Context::getDistance(){
  return distance;
}

void Context::setDroneDetected(bool detected){
  droneDetected = detected;
}

bool Context::isDroneDetected(){
  return droneDetected;
}
