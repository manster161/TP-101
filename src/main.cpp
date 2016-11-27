#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WebServer.h>
#include <TickerScheduler.h>
#include "tp101.h"


#define STATISTICS_TIMER 10000


extern char* global_timezoneDbApiKey;
char buffer[256];

ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE, 11);
TickerScheduler scheduler(1);
Tp101* tp101;


void HandleRoot() {
  server.send(200, "text/plain", "Hello from TP-101.\nRead the statistcs at /statistics");
  delay(100);
}

void Statistics() {
  server.send(200, "application/json", tp101->GetStatus(buffer, 256));
}

bool SetupServer(){
  server.on("/", HandleRoot);
  server.on("/statistcs", Statistics);
  server.begin();
  Serial.println("HTTP server started");

}

void updateStatistics(){
  Serial.println("UpdateStatistics");
  tp101->UpdateStatistics();
  Serial.println(tp101->GetStatus(buffer, 256));
  tp101->Handle();
}


void setup(void){
  Serial.begin(115200);


  Serial.println("Setup relays");
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  pinMode(RELAY3PIN, OUTPUT);
  pinMode(RELAY4PIN, OUTPUT);
  Serial.println("Create network");
  Network* network = new Network(&server);
Serial.println("Create TP");
  tp101 = new Tp101(network);
  Serial.println("Init TP");
  tp101->Init();
  Serial.println("Setup done");
  scheduler.add(0, STATISTICS_TIMER,updateStatistics);
}

void loop(void){
  scheduler.update();
  server.handleClient();
}
