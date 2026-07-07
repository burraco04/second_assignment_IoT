#include "HWPlatform.h"
#include <Arduino.h>
#include "devices/ButtonImpl.h"
#include "kernel/MsgService.h"
#include "config.h"
#include "devices/Led.h"
#include "devices/Pir.h"
#include "devices/Sonar.h"
#include "devices/LightSensorImpl.h"
#include "devices/ServoMotorImpl.h"
#include "devices/TempSensorTMP36.h"
#include "kernel/Logger.h"

void wakeUp(){}

HWPlatform::HWPlatform(){
  pButton = new ButtonImpl(BT_PIN);
  pLed1 = new Led(L1_PIN);
  pLed2 = new Led(L2_PIN);
  pLed3 = new Led(L3_PIN);
  pMotor = new ServoMotorImpl(MOTOR_PIN);
  pTempSensor = new TempSensorTMP36(TEMP_PIN);
  pPir = new Pir(PIR_PIN);
  pSonar = new Sonar(SONAR_ECHO_PIN, SONAR_TRIG_PIN, 30000);
}


void HWPlatform::init(){
}

Button* HWPlatform::getButton(){
  return this->pButton;
}


Led*  HWPlatform::getLed(){
  return this->pLed1;
}

Led* HWPlatform::getL1(){
  return this->pLed1;
}

Led* HWPlatform::getL2(){
  return this->pLed2;
}

Led* HWPlatform::getL3(){
  return this->pLed3;
}

ServoMotor* HWPlatform::getMotor(){
  return this->pMotor;
}

TempSensor* HWPlatform::getTempSensor(){
  return this->pTempSensor;
}

Pir* HWPlatform::getPir(){
  return this->pPir;
}

Sonar* HWPlatform::getSonar(){
  return this->pSonar;
}

void HWPlatform::test(){
  bool btPressed = pButton->isPressed();
  pLed1->switchOn();
  pLed2->switchOn();
  pLed3->switchOn();
  pMotor->on();
  pMotor->setPosition(90);
  Logger.log("Button: " + String(btPressed ? "pressed" : " not pressed"));
  delay(1000);
  pMotor->setPosition(0);
  delay(1000);
  pMotor->off();
  pLed1->switchOff();
  pLed2->switchOff();
  pLed3->switchOff();
}

