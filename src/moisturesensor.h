#ifndef _MOISTURESENSOR_H_
#define _MOISTURESENSOR_H_
#include "interfaces/analogsensor.h"



class MoistureSensor  {
public:
  MoistureSensor();
  ~MoistureSensor();
   bool init(void);
   float read(void);
  float GetMoistureLevel();
private:

};

#endif
