#include "tp101.h"
#include "pins.h"
#include "config/secrets.h"
Tp101::Tp101(){
  r1 = new Relay(RELAY1);
  r2 = new Relay(RELAY2);
  r3 = new Relay(RELAY3);
  r4 = new Relay(RELAY4);
  dht = new DHT(DHTPIN, DHTTYPE, 11);
  server = new ESP8266WebServer(80);
  wifiClient = new WiFiClient();
  httpClient = new HTTPClient();
}


void Tp101::Init(){



}
