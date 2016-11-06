#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "pins.h"





DHT dht(DHTPIN, DHTTYPE, 11);

void setup(void){

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

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
