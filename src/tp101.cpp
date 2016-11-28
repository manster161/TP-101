#include "tp101.h"

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

extern char* global_thingSpeakApiKey;

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
}

float Tp101::GetTemperature(){
  return _temperature;
}

float Tp101::GetHumidity(){
  return _humidity;
}

void Tp101::Handle(){
  if (_temperature < _minTemp && !r2->IsOn()){
    Serial.println("Its to coold, turning on heater");
    r2->On();
  }
  if (_temperature > _maxTemp && r2->IsOn()){
    Serial.println("Its hot again, turning off heater");
    r2->Off();
  }

  if (timeservice->GetCurrentHour() > lightsOn &&  timeservice->GetCurrentHour() <= lightsOff && !r1->IsOn()){
      Serial.println("Its day now, letting the sun come out");
    r1->On();
  } else if (timeservice->GetCurrentHour() < lightsOn &&  timeservice->GetCurrentHour() > lightsOff && r1->IsOn()){
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
  float hum = dht->readHumidity();

  if (hum >= 0 && hum <= 100)
    _humidity = hum;

  float temp =  dht->readTemperature(false);     // Read humidity (percent)
  if (temp >= 0 && temp <= 100)
    _temperature = temp;
  _moisture = _moisturesensor->Read();

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

      network->UpdateThingspeak(_temperature, _humidity, heating, lights, water);
  }
  else{
    int secondsTillUpdate = (int) (_postInterval - elapsedTime)/1000;
    Serial.printf("%d Seconds till next update\n", secondsTillUpdate);
  }

}
