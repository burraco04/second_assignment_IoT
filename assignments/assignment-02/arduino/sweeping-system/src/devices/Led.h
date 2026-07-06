#ifndef __LED__
#define __LED__

#include "Light.h"

class Led: public Light { 
public:
  Led(int pin);
  void switchOn();
  void switchOff();    
  bool isOn();
protected:
  bool on;
  int pin;  
};

#endif
