#ifndef _TP_H
#define _TP_H
#include "relay.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
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
  ESP8266WiFiMulti* WiFiMulti;
  ESP8266WebServer* server;
  HTTPClient* httpClient;
  WiFiClient* wifiClient;
};


#endif
