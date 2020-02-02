#ifndef TIMECOP_H
#define TIMECOP_H

#include <WiFiUdp.h>
#include <time.h>
#include <WiFi.h>

#define DEBUGMODE

#ifndef DEBUGMODE
#define DEBUG_PRINT(X) 
#define INFO_PRINT(X)
#else
#define DEBUG_PRINT(X) Serial.print("DEBUG: ");Serial.println(X);
#define INFO_PRINT(X) Serial.print("INFO: ");Serial.println(X);
#endif

#define NTPSERVER "pool.ntp.org"
#define UDPPORT 8888
#define NTPTIMEOUT 1000


void sendNTPpacket(IPAddress &address);

bool getNTPTime(time_t &t);
void printTime(time_t &t);

void getDeviceTime(time_t &t);
void renderTime(time_t &t, char*);

#endif