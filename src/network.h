#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>

class Network {
public:
  Network();
  bool Init(ESP8266WebServer* , WiFiClient* ,HTTPClient* , ESP8266WiFiMulti* );
  int ScanNetworks(void);
  bool ConnectToNetwork(const char*, const char*);
  void HandleRoot(void);
  bool SetupServer(void);
  void UpdateThingspeak(int temp, int humidity,int moisture, int heating, int lights, int water);
  String  GetIp();
  const char* GetNetwork();
  WiFiClient* GetWiFiClient();

private:
  const char * IpToCharArray();
  void Post(String);
  ESP8266WebServer* server;
  WiFiClient* wifiClient;
  HTTPClient* httpClient;
  ESP8266WiFiMulti* wifiMulti;
};

#endif
