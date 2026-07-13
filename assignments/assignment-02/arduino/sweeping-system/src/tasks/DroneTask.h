#ifndef __DRONE_TASK__
#define __DRONE_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include <Arduino.h>

class DroneTask: public Task {

public:
  DroneTask(Context* pContext);
  void tick();

private:
  enum State { RESTING, TAKING_OFF, FLYING, LANDING, SUSPENDED } state;

  void setState(State state);
  void syncStateFromContext();
  State stateFromContext(Context::DroneState state);
  bool checkAndSetJustEntered();
  void log(const String& msg);
  String stateToString(State state);
  void readCommand();
  void applyCommand(const String& command);
  void updateResting();
  void updateTakingOff();
  void updateFlying();
  void updateLanding();
  void updateSuspended();
  void resetConditionTimer();
  bool conditionHeldFor(long duration);
  bool isDroneOut();
  bool isDroneLanded();
  bool isDroneOutsideAfterSuspension();
  void updateContextState(State state);
  void notifyState();

  long stateTimestamp;
  long conditionTimestamp;
  bool justEntered;
  bool takeOffSignal;
  bool landingSignal;
  Context* pContext;
};

#endif
