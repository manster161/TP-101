  #include "moisturesensor.h"


  MoistureSensor::MoistureSensor(){

  }
  MoistureSensor::~MoistureSensor(){

  }


  float MoistureSensor::read(){

    return 0;
  }

 bool MoistureSensor::init(){
   pinMode(A0, INPUT);
   return true;
 }

  float MoistureSensor::GetMoistureLevel(){
      return 0;
  }
