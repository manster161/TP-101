#ifndef _TP_H_
#define _TP_H_
#include "relay.h"
#include <DHT.h>
#include "network.h"
#include <ArduinoJson.h>
#include "moisturesensor.h"
#include "timeservice.h"
#include "settings.h"

class Tp101{
public:
  Tp101(Network*);
  ~Tp101(void);
  void Init();
  void UpdateStatistics();
  void HandlePID();
  void Handle();
  float GetTemperature();
  float GetHumidity();
  char* GetStatus(char* buffer, size_t bufferSize);
  const char* GetNetwork();

private:
  StaticJsonBuffer<200> jsonBuffer;
  Relay* r1;
  Relay* r2;
  Relay* r3;
  Relay* r4;
  DHT* dht;
  Network* network;
  MoistureSensor* _moisturesensor;
  TimeService* timeservice;
  int foundNetworks = 0;
  int _humidity, _temperature, _moisture;
  int _maxTemp = 25;
  int _minTemp = 20;
  unsigned long _previousMillis = 0;
  unsigned long _postInterval = 60000;
};


#endif
