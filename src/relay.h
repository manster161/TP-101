#ifndef RELAY_H_
#define RELAY_H_

class Relay{
public:
  Relay(char pin, const char*);
  ~Relay();
  void On(void);
  void Off(void);
  bool IsOn(void);
  char*  GetStatus();
private:
  char _pin = 0;
  bool _isOn = false;
  char _buffer[20];
  char _namebuffer[10];
};

#endif
