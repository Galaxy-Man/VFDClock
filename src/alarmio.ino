#include <Arduino.h>
#include <definitions.h>
#include <WiFi.h>
#include <time.h>
#include <TimeCop.h>
#include <SPI.h>
#include <renderer.h>
#include "Ticker.h"
#include <Weatherman.h>

time_t currentTime;

 Ticker NTPChecker(NTPUpdate, 1000*60*5, MILLIS);
Ticker WeatherChecker(weatherUpdate, 1000*60*15, MILLIS);

weatherInfo currentWeather;
bool weatherValid = false;


void setup() {
    
    #ifdef DEBUGMODE 
    Serial.begin(115200);
    #endif
    
    WiFi.begin(ssid, password);
    int start = millis();
    while (WiFi.status() != WL_CONNECTED && millis()-start < 5000) {
        delay(1000);
        DEBUG_PRINT("Connecting to WiFi..");
    }
    if(WiFi.status() == WL_CONNECTED){
        INFO_PRINT("Connected to the WiFi network");
    }
    else{
        displayRebooting();
        ESP.restart();
    }

    initDisp();

    while(!getNTPTime(currentTime)){
        displayUnableToConnectMsg();
        delay(5000);
    }

    if(getNewWeather(currentWeather)){
        weatherValid = true;
    }

    NTPChecker.start();
    WeatherChecker.start();
    dispBrightness(4);
}


void dispWeather(WeatherInfo w){
    INFO_PRINT(w.currentTemp)
    INFO_PRINT(w.tempMax);
    INFO_PRINT(w.tempMin);
    printf("Sunrise : %02d:%02d\n", hour(w.sunrise) , minute(w.sunrise));
    printf("Sunset : %02d:%02d\n", hour(w.sunset) , minute(w.sunset));
}



void loop() {    
    for(int i = 0; i < 300; i++){
        getDeviceTime(currentTime);
        if(weatherValid){
            displayTime(currentTime, currentWeather);
        }
        else{
            displayTime(currentTime);
        }
        
        delay(200);
        if(second(currentTime) == 0 && minute(currentTime)%15 == 0){
            invertDisp();
        }
    }
    getNTPTime(currentTime);

    
}

void NTPUpdate(){
    getNTPTime(currentTime);
}

void weatherUpdate(){
    getNewWeather(currentWeather);
}