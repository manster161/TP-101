#include "display.h"



char upperBuffer[16];
char lowerBuffer[16];
LiquidCrystal_I2C lcd(0x3F, 20, 4);



Display::Display(){

}


void Display::Init(){
  lcd.begin();
}

void Display::Print(char* line1, char* line2){

  memccpy_P(upperBuffer, line1, 0, 16);
  memccpy_P(lowerBuffer, line2, 0, 16);
  lcd.setCursor(0,0);
  lcd.print(upperBuffer);
  lcd.setCursor(0,1);
  lcd.print(lowerBuffer);
}
