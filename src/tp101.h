#ifndef _TP_H_
#define _TP_H_
#include "relay.h"
#include <DHT.h>
#include "network.h"
#include <ArduinoJson.h>
#include "moisturesensor.h"

class Tp101{
public:
  Tp101(Network*);
  ~Tp101(void);
  void Init();
  void UpdateStatistics();
  float GetTemperature();
  float GetHumidity();
private:
  StaticJsonBuffer<200> jsonBuffer;
  Relay* r1;
  Relay* r2;
  Relay* r3;
  Relay* r4;
  DHT* dht;
  Network* network;
  MoistureSensor* _moisturesensor;
  int foundNetworks = 0;
  float _humidity, _temperature, _moisture;

};


#endif
