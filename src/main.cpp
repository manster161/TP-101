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


void HandleRoot() {
  server.send(200, "text/plain", "Hello from TP-101.\nRead the statistcs at /stat");
  delay(100);
}

void Statistics() {
  server.send(200, "application/json", tp101.GetStatus(buffer, 2048));
}

bool SetupServer(){
  server.on("/", HandleRoot);
  server.on("/stat", Statistics);
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
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  pinMode(RELAY3PIN, OUTPUT);
  pinMode(RELAY4PIN, OUTPUT);

  Serial.println("Init TP");
  network.Init(&server, &wifiClient, &httpClient, &wifiMulti);
  tp101.Init(&network);// = new Tp101(network);

  Serial.println("Starting webserver");
  SetupServer();
  Serial.println("Setup done");

  scheduler.add(0, STATISTICS_TIMER,updateStatistics);

}

void loop(void){
  scheduler.update();
  server.handleClient();
}
