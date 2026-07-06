#ifndef __CONTEXT__
#define __CONTEXT__

class Context {

public:
  enum DroneState {
    RESTING,
    TAKING_OFF,
    FLYING,
    LANDING,
    SUSPENDED
  };

  Context();

  void setResting();
  void setTakingOff();
  void setFlying();
  void setLanding();
  void setSuspended();
  void clearSuspended();

  DroneState getDroneState();
  bool isResting();
  bool isTakingOff();
  bool isFlying();
  bool isLanding();
  bool isSuspended();

  void setStarted();
  void setStopped();

  bool isStarted();  
  bool isStopped();
  void reset();

private:

  bool started; 
  bool stopped;
  DroneState droneState;
  DroneState resumeState;
};

#endif
