#include "tp101.h"
#include "pins.h"



Tp101::Tp101(Network* network){
  r1 = new Relay(RELAY1, "Lights");
  r1->Off();
  r2 = new Relay(RELAY2, "Heating");
  r2->Off();
  r3 = new Relay(RELAY3, "Water");
  r3->Off();
  r4 = new Relay(RELAY4, "Air");
  r4->Off();
  dht = new DHT(DHTPIN, DHTTYPE, 11);
  _moisturesensor = new MoistureSensor();
  this->network = network;
}

void Tp101::Init(){
  network->Init();
  Serial.println("network initialization done");
  _moisturesensor->init();
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
}
void Tp101::UpdateStatistics(){
  float hum = dht->readHumidity();

  if (hum >= 0 && hum <= 100)
    _humidity = hum;

  float temp =  dht->readTemperature(false);     // Read humidity (percent)
  if (temp >= 0 && temp <= 100)
    _temperature = temp;

  _moisture = _moisturesensor->read();

    Serial.println("TP update statistics");
    Serial.println("Temperature: " + String((int)_temperature) );
    Serial.println("Humidity: " + String((int)_humidity) );
    Serial.println("Moisture: " + String((int)_moisture) );

    Serial.println(r1->GetStatus());
    Serial.println(r2->GetStatus());
    Serial.println(r3->GetStatus());
    Serial.println(r4->GetStatus());


    unsigned long currentMillis = millis();
    unsigned long elapsedTime = currentMillis - _previousMillis;
    if(elapsedTime  >= _postInterval) {
      // save the last time you read the sensor
      _previousMillis = currentMillis;
      network->Post(_temperature, _humidity);
  }
  else{
    int secondsTillUpdate = (int) (_postInterval - elapsedTime)/1000;
    Serial.printf("%d Seconds till next update\n", secondsTillUpdate);
  }

}
