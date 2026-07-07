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

  enum HangarState {
    NORMAL,
    PRE_ALARM,
    ALARM
  };

  Context();

  void setResting();
  void setTakingOff();
  void setFlying();
  void setLanding();
  void setSuspended();
  void clearSuspended();
  void setHangarNormal();
  void setHangarPreAlarm();
  void setHangarAlarm();
  void setAlarm();
  void clearAlarm();

  DroneState getDroneState();
  HangarState getHangarState();
  bool isResting();
  bool isTakingOff();
  bool isFlying();
  bool isLanding();
  bool isSuspended();
  bool isNormal();
  bool isPreAlarm();
  bool isAlarm();

  void setTemperature(float temp);
  float getTemperature();

  void setStarted();
  void setStopped();

  bool isStarted();  
  bool isStopped();
  void reset();

  void setDistance(float distance);
  float getDistance();
  void setDroneDetected(bool detected);
  bool isDroneDetected();

private:

  bool started; 
  bool stopped;
  bool droneDetected;
  DroneState droneState;
  DroneState resumeState;
  HangarState hangarState;
  float temperature;
  float distance;
};

#endif
