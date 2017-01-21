#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //This library you can add via Include Library > Manage Library >


class Display
{
public:
  Display();
  ~Display();
  void Init(void);
  void Print(char*, char*);
private:

};
#endif
