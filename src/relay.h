#ifndef RELAY_H_
#define RELAY_H_

class Relay{
public:
  Relay(char pin, const char*);
  void On(void);
  void Off(void);
  bool IsOn(void);
  char*  GetStatus(char*);
  long OpenTimeSinceReset();
  char GetPin();
private:
  const char * _name;
  char _pin;
  bool _isOn = false;
  long _startMillis, _totalRunningTime;
};

#endif
