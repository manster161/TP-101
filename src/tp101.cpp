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

char localTimeBuffer[20];
int lightsOn = 7;
int lightsOff = 23;
int windowSize = 5000;
long heaterStartTime;
long waterStartTime;

int waterWindowSize = 5000;
int heaterWindowSize = 5000;


extern char* global_thingSpeakApiKey;
char tpTextBuffer[256];
double _temperature, heaterOuput;
double _moisture, moistureOuput;
double heaterSetpoint = 24;
double moistureSetpoint = 70;
double _humidity;

int foundNetworks = 0;
unsigned long _previousMillis = 0;
unsigned long _postInterval = 60000;



MoistureSensor moisturesensor;
TimeService timeservice;
PID pidArray [] = {
  PID(&_temperature, &heaterOuput, &heaterSetpoint, 2,5,1, DIRECT),
  PID(&_moisture, &moistureOuput, &moistureSetpoint, 2,5,1, DIRECT),
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
  SetPid(HEATER, p, i, d);
}

void GetPid(char* buffer,int pidIndex){
  sprintf(buffer,"p: %d, i: %d, d: %d", pidArray[pidIndex].GetKp(), pidArray[pidIndex].GetKi(), pidArray[pidIndex].GetKd());
}
void Tp101::GetHeaterPid(char * buffer){
  GetPid(buffer,HEATER);
}
void Tp101::GetWaterPid(char * buffer){
  GetPid(buffer,WATER);
}
void Tp101::SetWaterPid(double p, double i, double d){
  SetPid(WATER, p, i, d);
}
void Tp101::Init(Network* network){

  relays[LIGHT].Off();
  relays[HEATER].Off();
  relays[AIR].Off();
  relays[WATER].Off();
  this->network = network;
  timeservice.Init(network->GetWiFiClient());
  Serial.println("network initialization done");
  timeservice.UpdateTime();
  heaterStartTime= millis();
  pidArray[HEATER].SetOutputLimits(0, 5000);
  pidArray[HEATER].SetMode(AUTOMATIC);
  pidArray[WATER].SetOutputLimits(0, 5000);
  pidArray[WATER].SetMode(AUTOMATIC);
}

double Tp101::GetTemperature(){
  return _temperature;
}

double Tp101::GetHumidity(){
  return _humidity;
}

void Tp101::ControlHeater(){

  float temp =  dht.readTemperature(false);     // Read humidity (percent)

  if (temp >= 0 && temp <= 100)
    _temperature = temp;

  pidArray[HeaterPID].Compute();

  unsigned long now = millis();

  if(now - heaterStartTime > heaterWindowSize)
  { //time to shift the Relay Window
    heaterStartTime += heaterWindowSize;
  }

  if(heaterOuput > now - heaterStartTime){
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

  _moisture = moisturesensor.Read();

  pidArray[WaterPID].Compute();
  unsigned long now = millis();

  if(now - waterStartTime > waterWindowSize)
  { //time to shift the Relay Window
    waterStartTime += waterWindowSize;
  }

  if(moistureOuput > now - waterStartTime){
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
  //ControlMoisture();
  //ControlHeater();
 }


void Tp101::Handle(){


  float temp =  dht.readTemperature(false);

  if (temp < 23){
    relays[HEATER].On();
  }
  if (temp > 24){
    relays[HEATER].Off();
  }

  _moisture = moisturesensor.Read();

  if (_moisture < 30){
    relays[WATER].On();
  }
  if (_moisture > 60){
    relays[WATER].On();
  }


  int currentHour = timeservice.GetCurrentHour();

  if (currentHour >= lightsOn && currentHour <= lightsOff)
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
    readings["temp"] = _temperature;
    readings["humidity"] = _humidity;
    readings["moisture"] = _moisture;

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


    timeObj["localtime"] = timeservice.GetLocalTime(localTimeBuffer, 19);
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

      network->UpdateThingspeak(_temperature, _humidity, _moisture,  heating, lights, water);
  }
  else
  {
    int secondsTillUpdate = (int) (_postInterval - elapsedTime)/1000;

  }
}
