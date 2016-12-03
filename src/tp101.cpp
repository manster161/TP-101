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

double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;


int moistureWindowSize = 5000;
long moistureWindowStartTime;

extern char* global_thingSpeakApiKey;

double heaterSetpoint, _temperature, heaterOuput;
double moistureSetpoint, _moisture, moistureOuput;
PID heaterPID(&_temperature, &heaterOuput, &heaterSetpoint, consKp,consKi,consKd, DIRECT);
PID moisturePID(&_moisture, &moistureOuput, &moistureSetpoint, consKp,consKi,consKd, DIRECT);

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
  moistureSetpoint = 70;

  heaterPID.SetOutputLimits(0, 5000);
  heaterPID.SetMode(AUTOMATIC);

  moisturePID.SetOutputLimits(0, 5000);
  moisturePID.SetMode(AUTOMATIC);

}

float Tp101::GetTemperature(){
  return _temperature;
}

float Tp101::GetHumidity(){
  return _humidity;
}

void Tp101::ControlHeater(){

  double gap = abs(moistureSetpoint - _moisture);

  if (gap < 5){
    heaterPID.SetTunings(consKp, consKi, consKd);
      Serial.println("Switching to conservative strategy heating");
  }
  else {
    Serial.println("Switching to aggressive strategy heating");
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
  //Serial.printf("%d - %d = %d > %d\n",now, windowStartTime, now - windowStartTime,  windowSize);
//  Serial.printf("moistureOutput:%d, now: %d, windowStartTime: %d\n",moistureOuput, now, windowStartTime );
  if(moistureOuput > now - moistureWindowStartTime){
    if (!r4->IsOn()){
      Serial.println("Let it rain!");
    }
    r4->On();
  }
  else {
    if (r4->IsOn()){
      Serial.println("Sun is shining, the Weather is sweat");
    }
    r4->Off();
}
}

void Tp101::HandlePID(){
  ControlMoisture();
  ControlHeater();
 }


void Tp101::Handle(){

  int currentHour = timeservice->GetCurrentHour();

//
Serial.printf("CurrentHour %d : lightsOn: %d lightsOff: %d\n", currentHour, lightsOn, lightsOff );
  if (currentHour >= lightsOn && currentHour <= lightsOff)
  {
    if (!r1->IsOn()){
        Serial.println("Its day now, letting the sun come out");
    }
    r1->On();
  } else {
    if (r1->IsOn()){
        Serial.println("Its night again, see you tomorrow");
    }
    r1->Off();
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

    if (r1->IsOn())
      readings["lamp"] = "on";
    else
      readings["lamp"] = "off";

      readings["lampRunningTime"] = r1->OpenTimeSinceReset();

      if (r2->IsOn())
        readings["heating"] = "on";
      else
        readings["heating"] = "off";

      readings["heatingRunningTime"] = r2->OpenTimeSinceReset();



      if (r4->IsOn())
        readings["pump"] = "on";
      else
        readings["pump"] = "off";

        readings["pumpRunningTime"] = r4->OpenTimeSinceReset();

    //JsonObject& timeObj = root.createNestedObject().createNestedObject("time");

    timeObj["localtime"] = timeservice->GetLocalTime(localTimeBuffer, 19);
    timeObj["timestamp"] = timeservice->GetTimestamp();


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
      else
        lights = 0;

      if (r2->IsOn()){
        heating = 1;
      }
      else
        heating = 0;

      if (r4->IsOn()){
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
