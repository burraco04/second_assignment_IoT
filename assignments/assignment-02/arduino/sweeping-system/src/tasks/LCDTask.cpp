#include "tasks/LCDTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

LCDTask::LCDTask(Context* pContext):
    pContext(pContext), lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS){
  lcd.init();
  lcd.backlight();
  setState(RESTING_VIEW);
}

void LCDTask::tick(){
  State nextState = resolveState();
  if (nextState != state){
    setState(nextState);
  }

  if (checkAndSetJustEntered()){
    printCurrentState();
  }
}

LCDTask::State LCDTask::resolveState(){
  if (pContext->isAlarm() || pContext->isSuspended()){
    return ALARM_VIEW;
  }
  if (pContext->isTakingOff()){
    return TAKE_OFF_VIEW;
  }
  if (pContext->isLanding()){
    return LANDING_VIEW;
  }
  if (pContext->isFlying()){
    return FLYING_VIEW;
  }
  return RESTING_VIEW;
}

void LCDTask::printCurrentState(){
  lcd.clear();
  lcd.setCursor(0, 0);

  switch (state){
    case RESTING_VIEW:
      log(F("[LCD] DRONE INSIDE"));
      lcd.print(F("DRONE INSIDE"));
      break;

    case TAKE_OFF_VIEW:
      log(F("[LCD] TAKE OFF"));
      lcd.print(F("TAKE OFF"));
      break;

    case FLYING_VIEW:
      log(F("[LCD] DRONE OUT"));
      lcd.print(F("DRONE OUT"));
      break;

    case LANDING_VIEW:
      log(F("[LCD] LANDING"));
      lcd.print(F("LANDING"));
      break;

    case ALARM_VIEW:
      log(F("[LCD] ALARM"));
      lcd.print(F("ALARM"));
      break;
  }
}

void LCDTask::setState(State s){
  state = s;
  stateTimestamp = millis();
  justEntered = true;
}

bool LCDTask::checkAndSetJustEntered(){
  bool bak = justEntered;
  if (justEntered){
    justEntered = false;
  }
  return bak;
}

void LCDTask::log(const String& msg){
  Logger.log(msg);
}
