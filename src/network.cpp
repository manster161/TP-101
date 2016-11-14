#include "network.h"
String webString;
ESP8266WebServer server(80);

 Network::Network(){
//  server(80);// = new ESP8266WebServer(80);
  wifiClient = new WiFiClient();
  httpClient = new HTTPClient();
  wifiMulti = new ESP8266WiFiMulti();
}

void Network::Init(void){
  int foundNetworks = ScanNetworks();

  for (int i = 0; i < foundNetworks; i++){
    if (WiFi.SSID(i) == Secrets::ssid){
      ConnectToNetwork(Secrets::ssid, Secrets::password, wifiMulti, httpClient);
      break;
    }
  }

}

bool Network::ConnectToNetwork(const char* ssid, const char* password,ESP8266WiFiMulti* wifiMulti, HTTPClient* httpClient){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  wifiMulti->addAP(ssid, password);
  httpClient->setReuse(true);

  Serial.print("\n\r \n\rWorking to connect");
  int timer = 0;
  while (wifiMulti->run() != WL_CONNECTED && timer < Settings::ConnectionTimeOut) {
    delay(500);
    timer += 500;
    Serial.print(".");
  }
  if (wifiMulti->run() != WL_CONNECTED){
    Serial.printf("Could not connect to network: %s\n", ssid);
    return false;
  }
  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

int Network::ScanNetworks(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("scan start");

  int foundNetworks = WiFi.scanNetworks();
  Serial.println("scan done");
  if (foundNetworks == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(foundNetworks);
    Serial.println(" networks found");
    for (int i = 0; i < foundNetworks; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }

  return foundNetworks;
}

void HandleRoot() {
  server.send(200, "text/plain", "Hello from the weather esp8266, read from /temp or /humidity");
  delay(100);
}

bool Network::SetupServer(){
  server.on("/", HandleRoot);

  server.begin();
  Serial.println("HTTP server started");

}
