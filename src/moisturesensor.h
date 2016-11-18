#ifndef _MOISTURESENSOR_H_
#define _MOISTURESENSOR_H_
#include "interfaces/analogsensor.h"



class MoistureSensor : public AnalogSensor {
public:
  MoistureSensor();
  ~MoistureSensor();
  virtual bool init(short);
  float GetMoistureLevel();
private:

};

#endif
