#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define DHTTYPE DHT11
#define DHTPIN  2

//tx  rx  gpio4 gpio5 A gpio16 gpio14 gpio12 gpio13

#define RELAY4PIN 13
#define RELAY3PIN 12 //Broken!!
#define RELAY2PIN 14
#define RELAY1PIN 16 //brun

namespace Settings {
 int ConnectionTimeOut  = 10000;
 float MaxTemp = 27;
 float MinTemp = 20;
}

#endif
