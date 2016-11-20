#include <Arduino.h>
#include "relay.h"



  Relay::Relay(char pin, const char* name){
    //Check if pin is valid
    _pin = pin;
    //Check lenght of name
    sprintf(_namebuffer, "%s", name);
    Serial.printf("Relay %s was created\n",_namebuffer);
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

    sprintf(_buffer, "%s is %s", _namebuffer, status);
    return _buffer;
  }
