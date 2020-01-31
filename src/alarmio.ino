#include <Arduino.h>
#include <definitions.h>
#include <Weatherman.h>
#include <WiFi.h>
#include <time.h>
#include <TimeCop.h>
#include <SPI.h>
#include <renderer.h>
#include "Ticker.h" 

time_t currentTime;



void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    
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

}


void dispWeather(WeatherInfo w){
    INFO_PRINT(w.currentTemp)
    INFO_PRINT(w.tempMax);
    INFO_PRINT(w.tempMin);
    printf("Sunrise : %02d:%02d\n", hour(w.sunrise) , minute(w.sunrise));
    printf("Sunset : %02d:%02d\n", hour(w.sunset) , minute(w.sunset));
}



void loop() {    
    for(int i = 0; i < 600; i++){
        getDeviceTime(currentTime);
        //printTime(currentTime);
        displayTime(currentTime);
        delay(100);
    }
    refreshDisplay();
    getNTPTime(currentTime);
    //weatherInfo w = getNewWeather();
    //dispWeather(w);
    
}

