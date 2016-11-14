#ifndef _TP_H_
#define _TP_H_
#include "relay.h"

#include <DHT.h>

class Tp101{
public:
  Tp101(void);
  ~Tp101(void);
  void Init();

private:
  Relay* r1;
  Relay* r2;
  Relay* r3;
  Relay* r4;
  DHT* dht;

};


#endif
