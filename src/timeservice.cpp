#include "TimeService.h"

StaticJsonBuffer<300> timeServiceJsonBuffer;
String timezonedb = "api.timezonedb.com";
extern const char* global_timezoneDbApiKey;

TimeService::TimeService(WiFiClient* client){
  this->client = client;
}

TimeService::~TimeService(){

}

char* TimeService::GetLocalTime(char* buffer, int bufferLength){
    time_t n = now();
    sprintf(buffer, "%d:%d:%d", hour(n), minute(n), second(n));
  return buffer;
}

long TimeService::GetTimestamp(){
  return now();
}

int TimeService::GetCurrentHour(){
  return hour(now());
}
int TimeService::GetCurrentMinute(){
    return hour(minute());
}
void TimeService::UpdateTime(){
    Serial.println("Connecting to get curent time");
    if (client->connect(timezonedb.c_str(), 80)){
      Serial.println("Connected");
      String s = "GET /v2/get-time-zone?key="
      + String(global_timezoneDbApiKey) + "&by=zone&format=json&zone=CET HTTP/1.1\r\nHost: "
      + timezonedb +"\r\nConnection: close\r\n\r\n";
      Serial.println(global_timezoneDbApiKey);
      client->println(s.c_str());

      unsigned long timeout = millis();
      while (client->available() == 0) {

      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client->stop();
        return;
      }
    }


  while(client->available()){
    String line = client->readStringUntil('\r');
    char ln[line.length() + 1];
    sprintf(ln, "%s", line.c_str());
    JsonObject& root = timeServiceJsonBuffer.parseObject(ln);

    if(root.success()){
        long ts = root["timestamp"].as<long>();
        Serial.printf("Timestamp: %d\n", ts);
        String formatted = root["formatted"].asString();
        Serial.println(formatted);
        setTime(ts);
        break;
      }
      else{
        //Serial.println(line);
      }
    }
  }
  else{
    Serial.println("Failed to connect");
  }
}
