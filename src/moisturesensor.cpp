  #include "moisturesensor.h"

  static unsigned int maxValue;
  static unsigned int minValue;

  MoistureSensor::MoistureSensor(){

  }
  MoistureSensor::~MoistureSensor(){

  }

  void MoistureSensor::Init(){
    pinMode(A0, INPUT);
    maxValue = 1023;
    minValue = 0;
  }


void MoistureSensor::UpdateCalibrationValues() {
  unsigned int currentValue = analogRead(A0);
  if (currentValue > maxValue){
    maxValue = currentValue;
  }
  if (currentValue < minValue){
    minValue = currentValue;
  }
}

  int MoistureSensor::Read(){
    unsigned int val = analogRead(A0);
    val = constrain(val, 650, 1023);
  	int soil = map(val, 650, 1023, 100, 0);

    Serial.print("Moisture:");
    Serial.println(soil);
    return soil;
  }
