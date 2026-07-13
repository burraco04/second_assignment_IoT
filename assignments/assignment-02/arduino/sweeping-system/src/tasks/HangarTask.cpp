#include "tasks/HangarTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

#define TEMP1 30.0
#define TEMP2 40.0
#define T3 3000
#define T4 5000

HangarTask::HangarTask(Context* pContext): pContext(pContext){
  preAlarmTimestamp = 0;
  alarmTimestamp = 0;
  setState(Context::NORMAL);
}

void HangarTask::tick(){
  state = pContext->getHangarState();

  switch (state){
    case Context::NORMAL: {
      if (checkAndSetJustEntered()){
        log(F("[HANGAR] NORMAL"));
        MsgService.sendMsg(String(F("HANGAR:NORMAL")));
      }
      updateHangarState();
      break;
    }
    case Context::PRE_ALARM: {
      if (checkAndSetJustEntered()){
        log(F("[HANGAR] PRE-ALARM"));
        MsgService.sendMsg(String(F("HANGAR:PRE_ALARM")));
      }
      updateHangarState();
      break;
    }
    case Context::ALARM: {
      if (checkAndSetJustEntered()){
        log(F("[HANGAR] ALARM"));
        MsgService.sendMsg(String(F("HANGAR:ALARM")));
      }
      updateHangarState();
      break;
    }
  }
}

void HangarTask::updateHangarState(){
  float temp = pContext->getTemperature();
  long now = millis();

  if (state == Context::ALARM){
    return;
  }

  if (temp >= TEMP2){
    if (alarmTimestamp == 0){
      alarmTimestamp = now;
    }
    if ((now - alarmTimestamp) >= T4){
      pContext->setHangarAlarm();
      preAlarmTimestamp = 0;
      setState(Context::ALARM);
      return;
    }
  } else {
    alarmTimestamp = 0;
  }

  if (temp >= TEMP1){
    if (preAlarmTimestamp == 0){
      preAlarmTimestamp = now;
    }
    if ((now - preAlarmTimestamp) >= T3){
      pContext->setHangarPreAlarm();
      setState(Context::PRE_ALARM);
    }
  } else {
    preAlarmTimestamp = 0;
    alarmTimestamp = 0;
    if (state == Context::PRE_ALARM){
      pContext->setHangarNormal();
      setState(Context::NORMAL);
    }
  }
}

void HangarTask::setState(Context::HangarState s){
  state = s;
  justEntered = true;
}

bool HangarTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void HangarTask::log(const String& msg){
  Logger.log(msg);
}
