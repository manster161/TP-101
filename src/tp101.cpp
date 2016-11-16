#include "tp101.h"
#include "pins.h"




Tp101::Tp101(Network* network){
  r1 = new Relay(RELAY1);
  r2 = new Relay(RELAY2);
  r3 = new Relay(RELAY3);
  r4 = new Relay(RELAY4);
  dht = new DHT(DHTPIN, DHTTYPE, 11);
  this->network = network;
}

void Tp101::Init(){
  network->Init();
}
