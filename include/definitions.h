#ifndef DEFS_H
#define DEFS_H




#define WIFITIMEOUT 5000

#define DEBUGMODE

#ifndef DEBUGMODE
#define DEBUG_PRINT(X) 
#define INFO_PRINT(X)
#else
#define DEBUG_PRINT(X) Serial.print("DEBUG: ");Serial.println(X);
#define INFO_PRINT(X) Serial.print("INFO: ");Serial.println(X);
#endif


// void dispWeather(weatherInfo w);
void NTPUpdate();
void weatherUpdate();

#define MILISPERMIN 1000*60

#endif