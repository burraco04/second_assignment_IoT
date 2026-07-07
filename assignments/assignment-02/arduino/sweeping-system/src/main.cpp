#include <Arduino.h>
#include "config.h"
#include "kernel/Scheduler.h"
#include "kernel/Logger.h"
#include "kernel/MsgService.h"
#include "model/Context.h"
#include "model/HWPlatform.h"
#include "tasks/TestHWTask.h"
#include "tasks/DroneTask.h"
#include "tasks/HangarTask.h"
#include "tasks/HangarDoorTask.h"
#include "tasks/DDDTask.h"
#include "tasks/DPDTask.h"
#include "tasks/ResetButtonTask.h"
#include "tasks/LEDsTask.h"
#include "tasks/TempTask.h"

// #define __TESTING_HW__

Scheduler sched;

HWPlatform* pHWPlatform;
Context* pContext;

void setup() {
  MsgService.init();
  sched.init(50);

  Logger.log(":::::: Smart Drone Hangar ::::::");
  
  pHWPlatform = new HWPlatform();
  pHWPlatform->init();

#ifndef __TESTING_HW__
  pContext = new Context();

  Task* pResetButtonTask = new ResetButtonTask(pHWPlatform->getButton(), pContext);
  pResetButtonTask->init(100);

  Task* pDPDTask = new DPDTask(pHWPlatform->getPir(), pContext);
  pDPDTask->init(500);

  Task* pDroneTask = new DroneTask(pContext);
  pDroneTask->init(100);

  Task* pTempTask = new TempTask(pHWPlatform->getTempSensor(), pContext);
  pTempTask->init(500);

  Task* pHangarTask = new HangarTask(pContext);
  pHangarTask->init(500);

  Task* pDDDTask = new DDDTask(pHWPlatform->getSonar(), pContext);
  pDDDTask->init(500);

  Task* pHangarDoorTask = new HangarDoorTask(pHWPlatform->getMotor(), pContext);
  pHangarDoorTask->init(100);

  Task* pLEDsTask = new LEDsTask(pHWPlatform->getL1(), pHWPlatform->getL2(), pHWPlatform->getL3(), pContext);
  pLEDsTask->init(500);

  sched.addTask(pResetButtonTask);
  sched.addTask(pDPDTask);
  sched.addTask(pDroneTask);
  sched.addTask(pTempTask);
  sched.addTask(pHangarTask);
  sched.addTask(pDDDTask);
  sched.addTask(pHangarDoorTask);
  sched.addTask(pLEDsTask);
#endif

#ifdef __TESTING_HW__
  /* Testing */
  Task* pTestHWTask = new TestHWTask(pHWPlatform);
  pTestHWTask->init(2000);
  sched.addTask(pTestHWTask);
#endif
}

void loop() {
    sched.schedule();
}
