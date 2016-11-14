

class AnalogSensor{
public:
  virtual void init(short) = 0;
  virtual short read() = 0;
};
