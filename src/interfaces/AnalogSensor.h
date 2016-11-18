#include <Adafruit_Sensor.h>

class AnalogSensor{
public:
  virtual bool init(short) = 0;
  virtual short read() = 0;
};
