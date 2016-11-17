#include <Arduino.h>
#include "relay.h"


  char _pin = 0;
  bool _isOn = false;
  const char* _name;
  char _buffer[20];
  Relay::Relay(char pin, const char* name){
    //Check if pin is valid
    _pin = pin;
    //Check lenght of name
    _name = name;

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

  char* Relay::GetStatus(){
    char status[4];

    if (IsOn()){
      sprintf(status, "%s", "on");
    }
    else{
      sprintf(status, "%s", "off");
    }

    sprintf(_buffer, "%s is %s", _name, status);
    return _buffer;
  }
