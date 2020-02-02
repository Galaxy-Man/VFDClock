#include <Weatherman.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WString.h>
#include <secrets.h>
#include <Strings.h>

StaticJsonDocument<1000> jsonObj;

HTTPClient http;


 bool getNewWeather(weatherInfo &w){
    
    http.begin(weatherEndpoint + weatherKey);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String payload = http.getString();
        DEBUG_PRINT(payload);
        w = processWeather(payload);
    }
    else {
        INFO_PRINT("Error on HTTP request for weather");
        return(false);
    }
    http.end();
    return(true);
}

weatherInfo processWeather(String payload){
    weatherInfo w;
    DEBUG_PRINT(payload.length());
    DeserializationError error = deserializeJson(jsonObj, payload);
    if(error){
        INFO_PRINT(DSRLZTN_ERR);
    }
    else{
        float temp = jsonObj["main"]["temp"];
        float tempMax = jsonObj["main"]["temp_max"];
        float tempMin = jsonObj["main"]["temp_min"];
        long sunriseUnix = jsonObj["sys"]["sunrise"];
        long sunsetUnix = jsonObj["sys"]["sunset"];

        temp -=273.15;
        tempMax -=273.15;
        tempMin -=273.15;

        w.tNow = int(temp);
        w.tMax = int(tempMax);
        w.tMin = int(tempMin);
        w.srise = time_t(sunriseUnix);
        w.sset = time_t(sunsetUnix);
    }
    return(w);
}


void dispWeather(WeatherInfo w){
    INFO_PRINT(w.tNow)
    INFO_PRINT(w.tMax);
    INFO_PRINT(w.tMin);
    printf(SUNRISE_SPRINT, hour(w.srise) , minute(w.srise));
    printf(SUNSET_SPRINT, hour(w.sset) , minute(w.sset));
}