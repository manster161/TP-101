#include "tp101.h"
#include <PID_v1.h>
#include <ArduinoJson.h>

StaticJsonBuffer<256> jsonBuffer;
JsonArray& root = jsonBuffer.createArray();
JsonObject& sensors = root.createNestedObject().createNestedObject("network");
JsonObject& readings = root.createNestedObject().createNestedObject("readings");
JsonObject& timeObj = root.createNestedObject().createNestedObject("time");

char localTimeBuffer[20];
int minTemp = 20;
int maxTemp = 25;
int lightsOn = 7;
int lightsOff = 23;
int windowSize = 5000;
long windowStartTime;
extern char* global_thingSpeakApiKey;

double heaterSetpoint, _temperature, heaterOuput;
PID heaterPID(&_temperature, &heaterOuput, &heaterSetpoint, 2,5,1, DIRECT);

Tp101::Tp101(Network* network){
  r1 = new Relay(RELAY1PIN, "Lights");
  r1->Off();
  r2 = new Relay(RELAY2PIN, "Heating");
  r2->Off();
  r3 = new Relay(RELAY3PIN, "Air");
  r3->Off();
  r4 = new Relay(RELAY4PIN, "Water");
  r4->Off();
  dht = new DHT(DHTPIN, DHTTYPE, 11);
  _moisturesensor = new MoistureSensor();
  timeservice = new TimeService(network->GetWiFiClient());
  this->network = network;
}

void Tp101::Init(){
  network->Init();
  Serial.println("network initialization done");

  timeservice->UpdateTime();

  windowStartTime= millis();

  heaterSetpoint = 24;
  heaterPID.SetOutputLimits(0, 5000);
  heaterPID.SetMode(AUTOMATIC);
}

float Tp101::GetTemperature(){
  return _temperature;
}

float Tp101::GetHumidity(){
  return _humidity;
}

void Tp101::HandlePID(){
    heaterPID.Compute();

  /************************************************
  * turn the output pin on/off based on pid output
  ************************************************/
 unsigned long now = millis();

 if(now - windowStartTime > windowSize)
 { //time to shift the Relay Window
   windowStartTime += windowSize;
 }
 Serial.printf("%d - %d = %d > %d\n",now, windowStartTime, now - windowStartTime,  windowSize);
 Serial.printf("heaterOutput:%d, now: %d, windowStartTime: %d\n",heaterOuput, now, windowStartTime );
 if(heaterOuput > now - windowStartTime){
   if (!r2->IsOn()){
     Serial.println("Switching on heating");
   }
   r2->On();
 }
 else {
   if (r2->IsOn()){
     Serial.println("Switching off heating");
   }
   r2->Off();
 }
}

void Tp101::Handle(){

  if (timeservice->GetCurrentHour() >= lightsOn &&  timeservice->GetCurrentHour() < lightsOff){
      Serial.println("Its day now, letting the sun come out");
    r1->On();
  } else if (timeservice->GetCurrentHour() < lightsOn &&  timeservice->GetCurrentHour() >=
   lightsOff){
    Serial.println("Its night again, see you tomorrow");
    r1->Off();
  }

  int moistureLevel = _moisturesensor->Read();
  if (moistureLevel < 30 && !r4->IsOn()){
    Serial.println("Earth is dry. Letting the rain fall");
    r4->On();
  }

  if (moistureLevel > 70 && r4->IsOn()){
    Serial.println("Earth is nice and moist again. Stopping rainfall");
    r4->Off();
  }
}

 char* Tp101::GetStatus(char* buffer, size_t bufferSize){

    //JsonArray& root = jsonBuffer.createArray();
    //JsonObject& sensors = root.createNestedObject().createNestedObject("network");
    sensors["ipaddress"] = network->GetIp();
    sensors["network"] = network->GetNetwork();

    //JsonObject& readings = root.createNestedObject().createNestedObject("readings");
    readings["temp"] = _temperature;
    readings["humidity"] = _humidity;
    readings["moisture"] = _moisture;

    //JsonObject& timeObj = root.createNestedObject().createNestedObject("time");

    timeObj["localtime"] = timeservice->GetLocalTime(localTimeBuffer, 19);
    timeObj["time"] = timeservice->GetTimestamp();

    root.prettyPrintTo(Serial);
    root.printTo(buffer, bufferSize);
    return buffer;
}

void Tp101::UpdateStatistics(){
  int hum = dht->readHumidity();

  if (hum >= 0 && hum <= 100)
    _humidity = hum;

  _moisture = _moisturesensor->Read();

  float temp =  dht->readTemperature(false);     // Read humidity (percent)
  if (temp >= 0 && temp <= 100)
    _temperature = temp;

    unsigned long currentMillis = millis();
    unsigned long elapsedTime = currentMillis - _previousMillis;
    if(elapsedTime  >= _postInterval) {
      // save the last time you read the sensor
      _previousMillis = currentMillis;
      int heating = 0;
      int lights = 0;
      int water = 0;

      if (r1->IsOn()){
        lights = 1;
      }
      if (r2->IsOn()){
        heating = 1;
      }
      if (r4->IsOn()){
        water = 1;
      }

      network->UpdateThingspeak(_temperature, _humidity, _moisture,  heating, lights, water);
  }
  else{
    int secondsTillUpdate = (int) (_postInterval - elapsedTime)/1000;
    Serial.printf("%d Seconds till next update\n", secondsTillUpdate);
  }

}
