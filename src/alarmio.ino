#include <Arduino.h>
#include <definitions.h>
#include <Weatherman.h>
#include <WiFi.h>
#include <time.h>
#include <TimeCop.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GP9002.h>




void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

    #ifdef DEBUGMODE 
    Serial.begin(115200);
    #endif
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        DEBUG_PRINT("Connecting to WiFi..");
    }
    INFO_PRINT("Connected to the WiFi network");
}


void dispWeather(WeatherInfo w){
    INFO_PRINT(w.currentTemp)
    INFO_PRINT(w.tempMax);
    INFO_PRINT(w.tempMin);
    printf("Sunrise : %02d:%02d\n", hour(w.sunrise) , minute(w.sunrise));
    printf("Sunset : %02d:%02d\n", hour(w.sunset) , minute(w.sunset));
}

time_t currentTime;

void loop() {
    if(getNTPTime(currentTime)){
        printTime(currentTime);
    }
    else{
        DEBUG_PRINT("unable to get network time");
        printTime(currentTime);
    }
    for(int i = 0; i < 240; i++){
        getDeviceTime(currentTime);
        printTime(currentTime);
        delay(1000);
    }
    //weatherInfo w = getNewWeather();
    //dispWeather(w);
    
}

