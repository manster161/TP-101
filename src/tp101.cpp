#include "tp101.h"
#include <PID_v1.h>
#include <ArduinoJson.h>

StaticJsonBuffer<4096> jsonBuffer;
JsonArray& root = jsonBuffer.createArray();
JsonObject& sensors = root.createNestedObject().createNestedObject("network");
JsonObject& readings = root.createNestedObject().createNestedObject("readings");
JsonObject& timeObj = root.createNestedObject().createNestedObject("time");

char localTimeBuffer[20];
int lightsOn = 7;
int lightsOff = 23;
int windowSize = 5000;
long windowStartTime;

double aggKp=1, aggKi=0.05, aggKd=0.5;
double consKp=0.5, consKi=0.01, consKd=0.1;

int moistureWindowSize = 5000;
long moistureWindowStartTime;

extern char* global_thingSpeakApiKey;
double _temperature, heaterOuput;
double _moisture, moistureOuput;
double heaterSetpoint = 24;
double moistureSetpoint = 70;
double _humidity;

int foundNetworks = 0;
unsigned long _previousMillis = 0;
unsigned long _postInterval = 60000;

PID heaterPID(&_temperature, &heaterOuput, &heaterSetpoint, consKp,consKi,consKd, DIRECT);
PID moisturePID(&_moisture, &moistureOuput, &moistureSetpoint, consKp,consKi,consKd, DIRECT);
Relay LigthRelay(13, "Lights");
Relay HeaterRelay(12, "Heating");
Relay VentilationRelay(14, "Ventilation");
Relay WaterRelay(16, "Water");
DHT dht(DHTPIN, DHTTYPE, 11);

MoistureSensor moisturesensor;
TimeService timeservice;

Tp101::Tp101(){


}
Tp101::~Tp101(){


}

void Tp101::Init(Network* network){

  LigthRelay.Off();

  HeaterRelay.Off();

  VentilationRelay.Off();

  WaterRelay.Off();

  this->network = network;

  timeservice.Init(network->GetWiFiClient());

  Serial.println("network initialization done");

  timeservice.UpdateTime();

  windowStartTime= millis();

  heaterPID.SetOutputLimits(0, 5000);
  heaterPID.SetMode(AUTOMATIC);

  moisturePID.SetOutputLimits(0, 5000);
  moisturePID.SetMode(AUTOMATIC);

}

double Tp101::GetTemperature(){
  return _temperature;
}

double Tp101::GetHumidity(){
  return _humidity;
}

void Tp101::ControlHeater(){

  double gap = abs(heaterSetpoint - _temperature);

  if (gap < 15){
    heaterPID.SetTunings(consKp, consKi, consKd);
      Serial.println("Conservative strategy heating");
  }
  else {
    Serial.println("Aggressive strategy heating");
    heaterPID.SetTunings(aggKp, aggKi, aggKd);
  }

  heaterPID.Compute();
  unsigned long now = millis();

  if(now - windowStartTime > windowSize)
  { //time to shift the Relay Window
    windowStartTime += windowSize;
  }
//  Serial.printf("%d - %d = %d > %d\n",now, windowStartTime, now - windowStartTime,  windowSize);
//  Serial.printf("heaterOutput:%d, now: %d, windowStartTime: %d\n",heaterOuput, now, windowStartTime );
  if(heaterOuput > now - windowStartTime){
    if (!HeaterRelay.IsOn()){
      Serial.println("Switching on heating");
    }
    HeaterRelay.On();
  }
  else {
    if (HeaterRelay.IsOn()){
      Serial.println("Switching off heating");
    }
    HeaterRelay.Off();
}
}

void Tp101::ControlMoisture(){


  double gap = abs(moistureSetpoint - _moisture);

  if (gap < 50){
    moisturePID.SetTunings(consKp, consKi, consKd);
    Serial.println("Switching to conservative strategy moisture");
  }
  else {
    moisturePID.SetTunings(aggKp, aggKi, aggKd);
    Serial.println("Switching to conservative strategy moisture");
  }

  moisturePID.Compute();
  unsigned long now = millis();

  if(now - moistureWindowStartTime > moistureWindowSize)
  { //time to shift the Relay Window
    moistureWindowStartTime += moistureWindowSize;
  }

  if(moistureOuput > now - moistureWindowStartTime){
    if (!WaterRelay.IsOn()){
      Serial.println("Let it rain!");
    }
    WaterRelay.On();
  }
  else {
    if (WaterRelay.IsOn()){
      Serial.println("Sun is shining, the Weather is sweat");
    }
    WaterRelay.Off();
}
}

void Tp101::HandlePID(){
  ControlMoisture();
  ControlHeater();
 }


void Tp101::Handle(){

  int currentHour = timeservice.GetCurrentHour();


  if (currentHour >= lightsOn && currentHour <= lightsOff)
  {
    if (!LigthRelay.IsOn()){
        Serial.println("Its day now, letting the sun come out");
    }
    LigthRelay.On();
  } else {
    if (LigthRelay.IsOn()){
        Serial.println("Its night again, see you tomorrow");
    }
    LigthRelay.Off();
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

    if (LigthRelay.IsOn())
      readings["lamp"] = "on";
    else
      readings["lamp"] = "off";

      readings["lampRunningTime"] = String(LigthRelay.OpenTimeSinceReset());

      if (HeaterRelay.IsOn())
        readings["heating"] = "on";
      else
        readings["heating"] = "off";

      readings["heatingRunningTime"] = String(HeaterRelay.OpenTimeSinceReset());



      if (WaterRelay.IsOn())
        readings["pump"] = "on";
      else
        readings["pump"] = "off";

        readings["pumpRunningTime"] = String(WaterRelay.OpenTimeSinceReset());

    //JsonObject& timeObj = root.createNestedObject().createNestedObject("time");

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

  _moisture = moisturesensor.Read();

  float temp =  dht.readTemperature(false);     // Read humidity (percent)

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

      if (LigthRelay.IsOn()){
        lights = 1;
      }
      else
        lights = 0;

      if (HeaterRelay.IsOn()){
        heating = 1;
      }
      else
        heating = 0;

      if (WaterRelay.IsOn()){
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
