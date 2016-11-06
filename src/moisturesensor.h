#ifndef _MOISTURESENSOR_H_
#define _MOISTURESENSOR_H_
class MoistureSensor{
public:
  MoistureSensor(short);
  ~MoistureSensor();
  float GetMoistureLevel();
};

#endif
