#include "tp101.h"
#include <PID_v1.h>
#include <ArduinoJson.h>


#define HeaterPID 0
#define WaterPID 1
#define LIGHT  0
#define HEATER 1
#define AIR 2
#define WATER 3



DHT dht(DHTPIN, DHTTYPE, 11);
StaticJsonBuffer<4096> jsonBuffer;
JsonArray& root = jsonBuffer.createArray();
JsonObject& sensors = root.createNestedObject().createNestedObject("network");
JsonObject& readings = root.createNestedObject().createNestedObject("readings");
JsonObject& timeObj = root.createNestedObject().createNestedObject("time");
extern char* global_thingSpeakApiKey;

double temperature, heaterOuput;
double moisture, moistureOuput;
double heaterSetpoint;
double moistureSetpoint;
int foundNetworks;

MoistureSensor moisturesensor;
TimeService timeservice;
PID pidArray [] = {
  PID(&temperature, &heaterOuput, &heaterSetpoint, 2,5,1, DIRECT),
  PID(&moisture, &moistureOuput, &moistureSetpoint, 0.25,2,0.15, DIRECT),
};

Relay relays[] = {
  Relay(13, "Lights"),
  Relay(12, "Heating"),
  Relay(14, "Ventilation"),
  Relay(16, "Water"),
};


Tp101::Tp101(){


}
Tp101::~Tp101(){


}


void SetPid(int pidIndex, double p, double i, double d){
  pidArray[pidIndex].SetTunings(p, i, d);
}
void Tp101::SetHeaterPid(double p, double i, double d){
  SetPid(HeaterPID, p, i, d);
}

void GetPid(char* buffer,int pidIndex){
  sprintf(buffer,"p: %d, i: %d, d: %d", pidArray[pidIndex].GetKp(), pidArray[pidIndex].GetKi(), pidArray[pidIndex].GetKd());
}

void Tp101::GetHeaterPid(char * buffer){
  GetPid(buffer,HeaterPID);
}

void Tp101::GetWaterPid(char * buffer){
  GetPid(buffer,WaterPID);
}

void Tp101::SetWaterPid(double p, double i, double d){
  SetPid(WaterPID, p, i, d);
}

void Tp101::Init(Network* network){

  moisturesensor.Init();

  heaterSetpoint = 24;
  moistureSetpoint = 70;
  foundNetworks = 0;

   _lightsOn  =  7;
   _lightsOff = 23;

  _windowSize = 5000;
  _waterWindowSize = 1000;
  _heaterWindowSize = 5000;
  _postInterval = 60000;
  _previousMillis = 0;
  _useHeaterPid = false;
  _useWaterPid = false;

  relays[LIGHT].Off();
  relays[HEATER].Off();
  relays[AIR].Off();
  relays[WATER].Off();
  this->network = network;
  timeservice.Init(network->GetWiFiClient());
  Serial.println("network initialization done");
  timeservice.UpdateTime();
  _heaterStartTime= millis();
  _waterStartTime = millis();
  pidArray[HEATER].SetOutputLimits(0, 5000);
  pidArray[HEATER].SetMode(AUTOMATIC);
  pidArray[WATER].SetOutputLimits(0, 5000);
  pidArray[WATER].SetMode(AUTOMATIC);
}

double Tp101::GetTemperature(){
  return temperature;
}

double Tp101::GetHumidity(){
  return _humidity;
}

void Tp101::ControlHeater(){

  float temp =  dht.readTemperature(false);     // Read humidity (percent)

  if (temp >= 0 && temp <= 100)
    temperature = temp;

  pidArray[HeaterPID].Compute();

  unsigned long now = millis();

  if(now - _heaterStartTime > _heaterWindowSize)
  { //time to shift the Relay Window
    _heaterStartTime += _heaterWindowSize;
  }

  if(heaterOuput > now - _heaterStartTime){
    if (!relays[HEATER].IsOn()){
      Serial.println("Switching on heating");
    }
    relays[HEATER].On();
  }
  else {
    if (relays[HEATER].IsOn()){
      Serial.println("Switching off heating");
    }
    relays[HEATER].Off();
}
}

void Tp101::ControlMoisture(){

  moisture = moisturesensor.Read();

  pidArray[WaterPID].Compute();
  unsigned long now = millis();

  if(now - _waterStartTime > _waterWindowSize)
  { //time to shift the Relay Window
    _waterStartTime += _waterWindowSize;
  }

  if(moistureOuput > now - _waterStartTime){
    if (!relays[WATER].IsOn()){
      Serial.println("Let it rain!");
    }
    relays[WATER].On();
  }
  else {
    if (relays[WATER].IsOn()){
      Serial.println("Sun is shining, the Weather is sweet");
    }
    relays[WATER].Off();
}
}

void Tp101::HandlePID(){

  if (_useWaterPid){
    ControlMoisture();
  }
  if (_useHeaterPid){
    ControlHeater();
  }

 }


void Tp101::Handle(){
  float temp =  dht.readTemperature(false);

  if (temp < 23){
    relays[HEATER].On();
    _useHeaterPid = false;
  } else if (temp > 25){
    _useHeaterPid = false;
    relays[HEATER].Off();
  }
  else{
    _useHeaterPid = true;
  }

  moisture = moisturesensor.Read();

  if (moisture < 30){
    relays[WATER].On();
    _useWaterPid = false;
  } else if (moisture > 60){
    relays[WATER].On();
    _useWaterPid = false;
  }
  else{
    _useWaterPid = true;
  }

  int currentHour = timeservice.GetCurrentHour();

  if (currentHour >= _lightsOn && currentHour <= _lightsOff)
  {
    if (!relays[LIGHT].IsOn()){
        Serial.println("Its day now, letting the sun come out");
    }
    relays[LIGHT].On();
  } else {
    if (relays[LIGHT].IsOn()){
        Serial.println("Its night again, see you tomorrow");
    }
    relays[LIGHT].Off();
  }
}

 char* Tp101::GetStatus(char* buffer, size_t bufferSize){

    sensors["ipaddress"] = network->GetIp();
    sensors["network"] = network->GetNetwork();
    readings["temp"] = GetTemperature();
    readings["humidity"] = GetHumidity();
    readings["moisture"] = moisturesensor.Read();

    if (relays[LIGHT].IsOn())
      readings["lamp"] = "on";
    else
      readings["lamp"] = "off";

      readings["lampRunningTime"] = String(relays[LIGHT].OpenTimeSinceReset());

    if (relays[HEATER].IsOn())
        readings["heating"] = "on";
    else
      readings["heating"] = "off";

    readings["heatingRunningTime"] = String(relays[HEATER].OpenTimeSinceReset());

    if (relays[WATER].IsOn())
      readings["pump"] = "on";
    else
      readings["pump"] = "off";

    readings["pumpRunningTime"] = String(relays[WATER].OpenTimeSinceReset());


    timeObj["localtime"] = timeservice.GetLocalTime(_localTimeBuffer, 19);
    timeObj["timestamp"] = String(timeservice.GetTimestamp());


    root.prettyPrintTo(Serial);
    root.printTo(buffer, bufferSize);
    return buffer;
}

void Tp101::UpdateStatistics(){
  int hum = dht.readHumidity();

  if (hum >= 0 && hum <= 100)
    _humidity = hum;

    unsigned long currentMillis = millis();
    unsigned long elapsedTime = currentMillis - _previousMillis;
    if(elapsedTime  >= _postInterval) {
      // save the last time you read the sensor
      _previousMillis = currentMillis;
      int heating = 0;
      int lights = 0;
      int water = 0;

      if (relays[LIGHT].IsOn()){
        lights = 1;
      }
      else
        lights = 0;

      if (relays[HEATER].IsOn()){
        heating = 1;
      }
      else
        heating = 0;

      if (relays[WATER].IsOn()){
        water = 1;
      }
      else water = 0;

      network->UpdateThingspeak(temperature, _humidity, moisture,  heating, lights, water);
  }
  else
  {
    int secondsTillUpdate = (int) (_postInterval - elapsedTime)/1000;

  }
}
