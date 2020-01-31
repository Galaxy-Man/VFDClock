#ifndef WEATHERMAN_H
#define WEATHERMAN_H
#include <WString.h>
#include <time.h>

#define DEBUGMODE

#ifndef DEBUGMODE
#define DEBUG_PRINT(X) 
#define INFO_PRINT(X)
#else
#define DEBUG_PRINT(X) Serial.print("DEBUG: ");Serial.println(X);
#define INFO_PRINT(X) Serial.print("INFO: ");Serial.println(X);
#endif


//#include <secrets.h>



typedef struct weatherInfo{
    int currentTemp;
    int tempMin;
    int tempMax;
    time_t sunrise;
    time_t sunset;
    String conditions;
}WeatherInfo;


bool getNewWeather(weatherInfo &w);
weatherInfo processWeather(String);

#endif