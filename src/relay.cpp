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

 long Relay::OpenTimeSinceReset(){
    return totalRunningTime;
  }


  void Relay::On(){

    if (!_isOn)
      startMillis = millis();

    digitalWrite(_pin, HIGH);
    _isOn = true;
  }

  void Relay::Off(){
    if (_isOn){
      totalRunningTime += millis() - startMillis;
      Serial.printf("Added runningtime to %s and is now %d\n", _namebuffer, totalRunningTime );
      totalRunningTime += millis() - startMillis;
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
