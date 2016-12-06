#ifndef _TIMESERVICE_H_
#define _TIMESERVICE_H_
#include <WiFiClient.h>
#include <Time.h>

#include <ArduinoJson.h>



class TimeService {
public:
   TimeService();
   ~TimeService();
   void Init(WiFiClient *);
   void UpdateTime();
   char*  GetLocalTime(char*, int);
   long GetTimestamp();
   int GetCurrentHour();
   int GetCurrentMinute();
private:

  WiFiClient* client;
};
#endif
