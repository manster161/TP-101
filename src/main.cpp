#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "pins.h"
#include <ESP8266WebServer.h>
#include "tp101.h"
#include "network.h"
#include <TickerScheduler.h>

#define STATISTICS_TIMER 60000

ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE, 11);
TickerScheduler scheduler(1);
Tp101* tp101;


void HandleRoot() {
  server.send(200, "text/plain", "Hello from the weather esp8266, read from /temp or /humidity");
  delay(100);
}

bool SetupServer(){
  server.on("/", HandleRoot);

  server.begin();
  Serial.println("HTTP server started");

}

void setup(void){

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  Network* network = new Network(&server);
  tp101 = new Tp101(network);

}
void updateStatistics(){
  tp101->UpdateStatistics();
}
void loop(void){
  scheduler.add(0, STATISTICS_TIMER,updateStatistics);
}
