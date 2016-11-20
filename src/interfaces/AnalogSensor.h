#include <Adafruit_Sensor.h>

class AnalogSensor{
public:
  virtual bool init(short) = 0;
  virtual float read() = 0;
};
