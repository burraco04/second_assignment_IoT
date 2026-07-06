#include "Led.h"
#include "Arduino.h"

Led::Led(int pin){
  this->pin = pin;
  this->on = false;
  pinMode(pin,OUTPUT);
}

void Led::switchOn(){
  digitalWrite(pin,HIGH);
  on = true;
}

void Led::switchOff(){
  digitalWrite(pin,LOW);
  on = false;
}

bool Led::isOn(){
  return on;
}
