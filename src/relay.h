#ifndef RELAY_H_
#define RELAY_H_

class Relay{
public:
  Relay(char pin);
  ~Relay();
  void On(void);
  void Off(void);
  bool IsOn(void);
};

#endif
