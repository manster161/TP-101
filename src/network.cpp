#include "network.h"
#include <stdio.h>

#include <String.h>

String webString;
String thingspeak = "api.thingspeak.com";

const char* ssid = "Alexanderplatz";
int connectionTimeout = 5000;
extern const char* global_thingSpeakApiKey;
extern int foundNetworks;

Network::Network(){

}

bool Network::Init(ESP8266WebServer* server, WiFiClient* wifiClient,HTTPClient* httpClient, ESP8266WiFiMulti* wifiMulti)
{

  this->server = server;
  this->wifiClient = wifiClient;
  this->httpClient = httpClient;
  this->wifiMulti = wifiMulti;

  foundNetworks = ScanNetworks();
  bool connected = false;
  for (int i = 0; i < foundNetworks; i++){
    Serial.printf("Checking network %n\n", i );
    if (WiFi.SSID(i) == ssid){
      connected =  ConnectToNetwork(ssid, "snopp161");
    }
  }
  return connected;
}

bool Network::ConnectToNetwork(const char* ssid, const char* password){
  Serial.println("WiFi.mode(WIFI_STA)");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

Serial.println("Add ap");
  wifiMulti->addAP(ssid, password);
  Serial.println("httpClient->setReuse(true)");
  httpClient->setReuse(true);

  Serial.print("\n\r \n\rWorking to connect");
  int timer = 0;
  while (wifiMulti->run() != WL_CONNECTED && timer < connectionTimeout) {
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


WiFiClient* Network::GetWiFiClient(){
  Serial.println("GetWiFiClient");
  return wifiClient;
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

const char * Network::IpToCharArray(){
    IPAddress addr = WiFi.localIP();
    String str = String(addr.toString());
    return str.c_str();
}

String Network::GetIp(){
  return WiFi.localIP().toString();
}

const char* Network::GetNetwork(){
  return ssid;
}



void Network::UpdateThingspeak(int temp, int humidity, int moisture, int heating, int lights, int water){

  Serial.println("Connecting");

  String postStr = String(global_thingSpeakApiKey);
  postStr +="&field1=";
  postStr += String(temp);
  postStr +="&field2=";
  postStr += String(humidity);
  postStr +="&field3=";
  postStr += String(moisture);
  postStr +="&field4=";
  postStr += String(heating);
  postStr +="&field5=";
  postStr += String(lights);
  postStr +="&field6=";
  postStr += String(water);
  postStr += "\r\n\r\n";

    if (wifiClient->connect(thingspeak.c_str(), 80)){

      Serial.println("Connected");
      wifiClient->print("POST /update HTTP/1.1\n");
      wifiClient->print("Host: api.thingspeak.com\n");
      wifiClient->print("Connection: close\n");
      wifiClient->print("X-THINGSPEAKAPIKEY: "+ String(global_thingSpeakApiKey) + "\n");
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
