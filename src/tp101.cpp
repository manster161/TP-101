#include "tp101.h"
#include "pins.h"




Tp101::Tp101(Network* network){
  r1 = new Relay(RELAY1, "Lights");
  r2 = new Relay(RELAY2, "Heating");
  r3 = new Relay(RELAY3, "Water");
  r4 = new Relay(RELAY4, "Air");
  dht = new DHT(DHTPIN, DHTTYPE, 11);
  this->network = network;
}

void Tp101::Init(){
  network->Init();
}

void Tp101::UpdateStatistics(){

}
