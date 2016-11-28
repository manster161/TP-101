#ifndef _MOISTURESENSOR_H_
#define _MOISTURESENSOR_H_
#include "interfaces/analogsensor.h"



class MoistureSensor  {
public:
  MoistureSensor();
  ~MoistureSensor();
  void UpdateCalibrationValues();
  int Read(void);

private:

};

#endif
