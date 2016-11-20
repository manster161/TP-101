#ifndef _TEMPHUMIDITYSENSOR_H_
#define _TEMPHUMIDITYSENSOR_H_
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTTYPE DHT11


class TempHumiditySensor{
public:
  TempHumiditySensor();
  ~TempHumiditySensor();
  bool init();
  char* GetStatus();
private:
  DHT* _dht;
};
#endif
