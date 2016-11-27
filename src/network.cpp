#include "network.h"
#include <stdio.h>
String webString;
#include "config/secrets.h"
#include "settings.h"
#include <String.h>
#include <ArduinoJson.h>


StaticJsonBuffer<300> networkJsonBuffer;

 Network::Network(ESP8266WebServer* server){
  this->server = server;
  wifiClient = new WiFiClient();
  httpClient = new HTTPClient();
  wifiMulti = new ESP8266WiFiMulti();
}

bool Network::Init(){
  int foundNetworks = ScanNetworks();
  bool connected = false;
  for (int i = 0; i < foundNetworks; i++){
    if (WiFi.SSID(i) == Secrets::ssid){
      connected =  ConnectToNetwork(Secrets::ssid, Secrets::password, wifiMulti, httpClient);
    }
  }
  return connected;
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

const char * Network::IpToCharArray(){
    IPAddress addr = WiFi.localIP();
    String str = String(addr.toString());
    return str.c_str();
}

const char * Network::GetIp(){
  return IpToCharArray();
}

const char* Network::GetNetwork(){
  return Secrets::ssid;
}

const char * Network::GetTime(){
  char buffer[20];
  sprintf(buffer, "%d:%d:%d", hour(), minute(), second());
  return String(buffer).c_str();
}

int Network::GetTimestamp(){
  return now();
}

void Network::UpdateTime(){
    Serial.println("Connecting to get curent  time");
    if (wifiClient->connect(Secrets::timezonedb.c_str(), 80)){
      Serial.println("Connected");
      String s = "GET /v2/get-time-zone?key="
      + Secrets::timezonedbApiKey + "&by=zone&format=json&zone=CET HTTP/1.1\r\nHost: "
      + Secrets::timezonedb +"\r\nConnection: close\r\n\r\n";
      wifiClient->println(s.c_str());

      unsigned long timeout = millis();
      while (wifiClient->available() == 0) {

      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        wifiClient->stop();
        return;
      }
    }

  while(wifiClient->available()){
    String line = wifiClient->readStringUntil('\r');
    char ln[line.length() + 1];
    sprintf(ln, "%s",line.c_str());
    JsonObject& root = networkJsonBuffer.parseObject(ln);

    if(root.success())
      {

        long ts = root["timestamp"].as<long>();
        Serial.printf("Timestamp: %d\n", ts);
        String formatted = root["formatted"].asString();

        Serial.println(formatted);
        setTime(ts);
      }
      else{
        Serial.print("Line decoding failed for :");
        Serial.println(line);
      }
    }
  }
}



void Network::UpdateThingspeak(float temp, float humidity){

  Serial.println("Connecting");

  String postStr = Secrets::thingSpeakApiKey;
  postStr +="&field1=";
  postStr += String(temp);
  postStr +="&field2=";

  postStr += String(humidity);
  postStr += "\r\n\r\n";

    if (wifiClient->connect(Secrets::thingspeak.c_str(), 80)){

      Serial.println("Connected");
      wifiClient->print("POST /update HTTP/1.1\n");
      wifiClient->print("Host: api.thingspeak.com\n");
      wifiClient->print("Connection: close\n");
      wifiClient->print("X-THINGSPEAKAPIKEY: "+ String(Secrets::thingSpeakApiKey) + "\n");
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
