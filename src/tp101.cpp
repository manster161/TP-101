#include "tp101.h"
#include "pins.h"




Tp101::Tp101(Network* network){
  r1 = new Relay(RELAY1, "Lights");
  r2 = new Relay(RELAY2, "Heating");
  r3 = new Relay(RELAY3, "Water");
  r4 = new Relay(RELAY4, "Air");
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
void Tp101::UpdateStatistics(){
  _humidity = dht->readHumidity();          // Read humidity (percent)
  _temperature = dht->readTemperature(false);
  _moisture = _moisturesensor->read();

    Serial.println("TP update statistics");
    Serial.println("Temperature: " + String((int)_temperature) );
    Serial.println("Humidity: " + String((int)_humidity) );
    Serial.println("Moisture: " + String((int)_moisture) );
}
