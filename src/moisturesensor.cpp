#include "moisturesensor.h"


  MoistureSensor::MoistureSensor(){

  }
  MoistureSensor::~MoistureSensor(){

  }

 bool MoistureSensor::init(short analogPin){

   return true;
 }

  float MoistureSensor::GetMoistureLevel(){
      return 0;
  }
