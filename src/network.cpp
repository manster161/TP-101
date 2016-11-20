#include "network.h"
#include <stdio.h>
String webString;
#include "config/secrets.h"
#include "settings.h"

 Network::Network(ESP8266WebServer* server){
  this->server = server;
  wifiClient = new WiFiClient();
  httpClient = new HTTPClient();
  wifiMulti = new ESP8266WiFiMulti();
}

bool Network::Init(){
  int foundNetworks = ScanNetworks();

  for (int i = 0; i < foundNetworks; i++){
    if (WiFi.SSID(i) == Secrets::ssid){
      return ConnectToNetwork(Secrets::ssid, Secrets::password, wifiMulti, httpClient);
    }
  }
  return false;
}

bool Network::ConnectToNetwork(const char* ssid, const char* password,ESP8266WiFiMulti* wifiMulti, HTTPClient* httpClient){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  wifiMulti->addAP(ssid, password);
  httpClient->setReuse(true);

  Serial.print("\n\r \n\rWorking to connect");
  int timer = 0;
  while (wifiMulti->run() != WL_CONNECTED && timer < Settings::ConnectionTimeOut) {
    delay(500);
    timer += 500;
    Serial.print(".");
  }
  if (wifiMulti->run() != WL_CONNECTED){
    Serial.printf("Could not connect to network: %s\n", ssid);
    return false;
  }
  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

int Network::ScanNetworks(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("scan start");

  int foundNetworks = WiFi.scanNetworks();
  Serial.println("scan done");
  if (foundNetworks == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(foundNetworks);
    Serial.println(" networks found");
    for (int i = 0; i < foundNetworks; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }

  return foundNetworks;
}


void Network::Post(float temp, float humidity){

  Serial.println("Connecting");
    if (wifiClient->connect(Secrets::host.c_str(), 80)){
      String postStr = Secrets::apiKey;
      postStr +="&field1=";
      postStr += String(temp);
      postStr +="&field2=";
      postStr += String(humidity);
      postStr += "\r\n\r\n";

      wifiClient->print("POST /update HTTP/1.1\n");
      wifiClient->print("Host: api.thingspeak.com\n");
      wifiClient->print("Connection: close\n");
      wifiClient->print("X-THINGSPEAKAPIKEY: "+ String(Secrets::apiKey) + "\n");
      wifiClient->print("Content-Type: application/x-www-form-urlencoded\n");
      wifiClient->print("Content-Length: ");
      wifiClient->print(postStr.length());
      wifiClient->print("\n\n");
      wifiClient->print(postStr);
      wifiClient->stop();
      Serial.println("Posted:" + postStr);
    }
    else
    {
      Serial.println("Connect fail");
    }
  }
