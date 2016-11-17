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
  const char*  name;
};

#endif
