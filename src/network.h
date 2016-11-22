#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>


class Network {
public:
  Network(ESP8266WebServer* server);
  bool Init(void);
  int ScanNetworks(void);
  bool ConnectToNetwork(const char*, const char*,ESP8266WiFiMulti*, HTTPClient*);
  void HandleRoot(void);
  bool SetupServer(void);
  void Post(float temp, float humidity);
  const char* GetIp();
  const char* GetNetwork();
private:
  const char * IpToCharArray();
  ESP8266WiFiMulti* wifiMulti;
  ESP8266WebServer* server;
  HTTPClient* httpClient;
  WiFiClient* wifiClient;
};

#endif
