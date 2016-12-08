#ifndef RELAY_H_
#define RELAY_H_

class Relay{
public:
  Relay(char pin, const char*);
  void On(void);
  void Off(void);
  bool IsOn(void);
  char*  GetStatus();
  long OpenTimeSinceReset();
  char GetPin();
private:
  char _pin;
  bool _isOn = false;
  char _buffer[20];
  char _namebuffer[10];
  long startMillis, totalRunningTime;
};

#endif
