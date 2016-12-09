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
  Tp101();
  ~Tp101(void);
  void Init(Network*);
  void UpdateStatistics();
  void HandlePID();
  void Handle();
  double GetTemperature();
  void ControlHeater();
  void ControlMoisture();
  double GetHumidity();
  char* GetStatus(char* buffer, size_t bufferSize);
  const char* GetNetwork();
  void SetHeaterPid(double, double, double);
  void SetWaterPid(double, double, double);
  void GetHeaterPid(char *);
  void GetWaterPid(char *); 

private:
  Network* network;
};


#endif
