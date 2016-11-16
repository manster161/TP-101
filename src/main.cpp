#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "pins.h"
#include <ESP8266WebServer.h>
#include "tp101.h"
#include "network.h"


ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE, 11);

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

  Network* network = new Network();
  tp101 = new Tp101();

}

void loop(void){

  digitalWrite(RELAY1, HIGH);
  delay(250);
  digitalWrite(RELAY1, LOW);
  delay(250);

  digitalWrite(RELAY1, HIGH);
  delay(250);
  digitalWrite(RELAY1, LOW);
  delay(250);


  digitalWrite(RELAY2, HIGH);
  delay(500);
  digitalWrite(RELAY2, LOW);
  delay(500);

  digitalWrite(RELAY3, HIGH);
  delay(500);
  digitalWrite(RELAY3, LOW);
  delay(500);

  digitalWrite(RELAY4, HIGH);
  delay(500);
  digitalWrite(RELAY4, LOW);
  delay(500);
}
