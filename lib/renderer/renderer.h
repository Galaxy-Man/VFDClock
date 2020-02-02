#ifndef RENDERER_H
#define RENDERER_H

#include <time.h>
#include <Weatherman.h>


#define MISOPIN 19
#define MOSIPIN 18
#define CLKPIN 21
#define CSPIN 4
#define DCPIN 5


#define DEBUGMODE

#ifndef DEBUGMODE
#define DEBUG_PRINT(X) 
#define INFO_PRINT(X)
#else
#define DEBUG_PRINT(X) Serial.print("DEBUG: ");Serial.println(X);
#define INFO_PRINT(X) Serial.print("INFO: ");Serial.println(X);
#endif

void initDisp();
void displayTime(time_t &t);
void displayTime(time_t &t, weatherInfo &w);
void displayUnableToConnectMsg();
void refreshDisplay();
void displayRebooting();
void dispBrightness(int v);
void invertDisp(bool);

#endif