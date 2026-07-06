#include <Arduino.h>
#include "config.h"
#include "kernel/Scheduler.h"
#include "kernel/Logger.h"
#include "kernel/MsgService.h"
#include "model/HWPlatform.h"
#include "tasks/TestHWTask.h"
#include "tasks/DroneTask.h"
#include "tasks/LEDsTask.h"

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

  Task* pDroneTask = new DroneTask(pContext);
  pDroneTask->init(100);

  Task* pLEDsTask = new LEDsTask(pHWPlatform->getL1(), pHWPlatform->getL2(), pHWPlatform->getL3(), pContext);
  pLEDsTask->init(500);

  sched.addTask(pDroneTask);
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
