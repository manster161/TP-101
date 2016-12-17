#include <Arduino.h>
#include "relay.h"



  Relay::Relay(char pin, const char* name){
    _pin = pin;
    _name = name;
    Serial.printf("Relay %s was created\n",_name);
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
  }

 long Relay::OpenTimeSinceReset(){
    return _totalRunningTime;
  }


  void Relay::On(){

    if (!_isOn)
      _startMillis = millis();

    digitalWrite(_pin, HIGH);
    _isOn = true;
  }

  void Relay::Off(){
    if (_isOn){
      _totalRunningTime += millis() - _startMillis;
    }

    digitalWrite(_pin, LOW);
    _isOn = false;
  }

  char Relay::GetPin(){
    return _pin;
  }

  bool Relay::IsOn(){
    return _isOn;
  }

  char* Relay::GetStatus(char* buffer){

    if (IsOn()){
      sprintf(buffer, "%s is %s", _name, "on");
    }
    else{
      sprintf(buffer, "%s is %s", _name, "off");
    }

    return buffer;
  }
