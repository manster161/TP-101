#include "TimeService.h"

StaticJsonBuffer<300> networkJsonBuffer;
String timezonedb = "api.timezonedb.com";
extern const char* global_timezoneDbApiKey;

TimeService::TimeService(WiFiClient* client){
  this->client = client;
}

TimeService::~TimeService(){

}

const char* TimeService::GetLocalTime(){
  char buffer[20];
  sprintf(buffer, "%d:%d:%d", hour(), minute(), second());
  return String(buffer).c_str();
}

long TimeService::GetTimestamp(){
  return now();
}

void TimeService::UpdateTime(){
    Serial.println("Connecting to get curent  time");
    if (client->connect(timezonedb.c_str(), 80)){
      Serial.println("Connected");
      String s = "GET /v2/get-time-zone?key="
      + String(global_timezoneDbApiKey) + "&by=zone&format=json&zone=CET HTTP/1.1\r\nHost: "
      + timezonedb +"\r\nConnection: close\r\n\r\n";
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
    sprintf(ln, "%s",line.c_str());
    JsonObject& root = networkJsonBuffer.parseObject(ln);

    if(root.success())
      {

        long ts = root["timestamp"].as<long>();
        Serial.printf("Timestamp: %d\n", ts);
        String formatted = root["formatted"].asString();

        Serial.println(formatted);
        setTime(ts);
      }
      else{
        Serial.print("Line decoding failed for :");
        Serial.println(line);
      }
    }
  }
}
