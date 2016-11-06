#include <Arduino.h>
#include "relay.h"


  char _pin = 0;
  bool _isOn = false;

  Relay::Relay(char pin){
    _pin = pin;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
  }

  void Relay::On(){
    if (_isOn)
      return;
    digitalWrite(_pin, HIGH);
    
    _isOn = true;
  }

  void Relay::Off(){
    if (!_isOn)
      return;
    digitalWrite(_pin, LOW);
    _isOn = false;
  }

  bool Relay::IsOn(){
    return _isOn;
  }
