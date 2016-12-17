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
  char _localTimeBuffer[20];

  long _heaterStartTime;
  long _waterStartTime;

  int _lightsOn;
  int _lightsOff;
  int _windowSize = 5000;
  int _waterWindowSize = 5000;
  int _heaterWindowSize = 5000;
  unsigned long _postInterval = 60000;
  unsigned long _previousMillis = 0;
  bool _useHeaterPid = false;
  bool _useWaterPid = false;
  double _humidity;
};


#endif
