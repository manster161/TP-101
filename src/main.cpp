#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WebServer.h>
#include <TickerScheduler.h>
#include "tp101.h"


#define STATISTICS_TIMER 1000
#define PID_TIMER 5000

extern char* global_timezoneDbApiKey;
char buffer[2048];

ESP8266WebServer server(80);
WiFiClient wifiClient; //= new WiFiClient();
HTTPClient httpClient; // = new HTTPClient();
ESP8266WiFiMulti wifiMulti;// = new ESP8266WiFiMulti();
Network network;

TickerScheduler scheduler(1);
TickerScheduler pidScheduler(2);
Tp101 tp101;

double heaterKp=2, heaterKi=5, heaterKd=1;
double waterKp=2, waterKi=5, waterKd=1;


bool IsNumeric(String s){
  for (int i = 0; i < s.length(); i++){
    if (!isDigit(s[0]))
    return false;
  }
  return true;
}

void HandleRoot() {
  server.send(200, "text/plain", "Hello from TP-101.\nRead the statistcs at /stat");
  delay(100);
}

void Statistics() {
  server.send(200, "application/json", tp101.GetStatus(buffer, 2048));
}

void Heater() {

  String p = server.arg("p");
  String i = server.arg("i");
  String d = server.arg("d");
  if (IsNumeric(p)){
    
    heaterKp = (double)p.toInt();
  }
  if (IsNumeric(i)){
    heaterKi = (double)i.toInt();
  }
  if (IsNumeric(i)){
    heaterKd = (double)d.toInt();
  }
  sprintf(buffer, "Current heater pid values p: %d %s i: %d %s d:%d %s\n",heaterKp, p.c_str(), heaterKi, i.c_str(), heaterKd, d.c_str());
  server.send(200, "text/plain", buffer);
}


void Pump() {
  String p = server.arg("p");
  String i = server.arg("i");
  String d = server.arg("d");
  if (IsNumeric(p)){
    heaterKp = (double)p.toInt();
  }
  if (IsNumeric(i)){
    heaterKi = (double)i.toInt();
  }
  if (IsNumeric(i)){
    heaterKd = (double)d.toInt();
  }
  sprintf(buffer, "Current pump pid values\vp: %d\ni: %d\nd:%d",heaterKp, heaterKi, heaterKd);
  server.send(200, "text/plain", buffer);
}

bool SetupServer(){
  server.on("/", HandleRoot);
  server.on("/stat", Statistics);
  server.on("/settings/heater", Heater);
  server.on("/settings/pump", Heater);
  server.begin();
  Serial.println("HTTP server started");

}

void updateStatistics(){
  Serial.println("UpdateStatistics");
  tp101.UpdateStatistics();
  tp101.Handle();
  tp101.HandlePID();
}


void setup(void){
  Serial.begin(115200);
  Serial.println("Setup relays");

  Serial.println("Init TP");
  network.Init(&server, &wifiClient, &httpClient, &wifiMulti);
  tp101.Init(&network);

  Serial.println("Starting webserver");
  SetupServer();
  Serial.println("Setup done");

  scheduler.add(0, STATISTICS_TIMER,updateStatistics);
}

void loop(void){
  scheduler.update();
  server.handleClient();
}
