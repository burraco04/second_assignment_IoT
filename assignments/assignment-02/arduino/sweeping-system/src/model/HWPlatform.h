#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.h"
#include "devices/Button.h"
#include "devices/Led.h"
#include "devices/Pir.h"
#include "devices/Sonar.h"
#include "devices/ServoMotor.h"
#include "devices/TempSensor.h"

class HWPlatform {

public:
  HWPlatform();
  void init();
  void test();

  Button* getButton();
  Led*  getLed();
  Led*  getL1();
  Led*  getL2();
  Led*  getL3();
  ServoMotor* getMotor();
  TempSensor* getTempSensor();
  Pir* getPir();
  Sonar* getSonar();

private:
  Button* pButton;
  Led* pLed1;
  Led* pLed2;
  Led* pLed3;
  ServoMotor* pMotor;
  TempSensor* pTempSensor;
  Pir* pPir;
  Sonar* pSonar;
  
};

#endif
