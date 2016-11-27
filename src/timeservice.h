#ifndef _TIMESERVICE_H_
#define _TIMESERVICE_H_
#include <WiFiClient.h>
#include <Time.h>

#include <ArduinoJson.h>



class TimeService {
public:
   TimeService(WiFiClient *);
   ~TimeService();

   void UpdateTime();
   char*  GetLocalTime(char*, int);
   long GetTimestamp();

private:

  WiFiClient* client;
};
#endif
