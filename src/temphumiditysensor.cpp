#include "temphumiditysensor.h"

TempHumiditySensor::TempHumiditySensor(){
  _dht = new DHT(2, DHTTYPE, 11);
}
TempHumiditySensor::~TempHumiditySensor(){

}
bool TempHumiditySensor::init(){

}

char* TempHumiditySensor::GetStatus(){
   
}
