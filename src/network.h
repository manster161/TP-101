#ifndef _NETWORK_H_
#define _NETWORK_H_
#include "config/secrets.h"
#include "settings.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

class Network {
public:
  Network();
  void Init(void);
  int ScanNetworks(void);
  bool ConnectToNetwork(const char*, const char*,ESP8266WiFiMulti*, HTTPClient*);
  void HandleRoot(void);
  bool SetupServer(void);

private:
  ESP8266WiFiMulti* wifiMulti;
  ///ESP8266WebServer& server;
  HTTPClient* httpClient;
  WiFiClient* wifiClient;
};

#endif
