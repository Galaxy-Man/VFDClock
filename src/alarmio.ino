//------------------------------ Includes ---------------------------------------
#include <definitions.h>
#include <WiFi.h>
#include <time.h>
#include <TimeCop.h>
#include <renderer.h>
#include "Ticker.h"
#include <Weatherman.h>
#include <Secrets.h>
#include <Strings.h>

//------------------------------- Global vars --------------------------------

Ticker NTPChecker(NTPUpdate, MILISPERMIN*10, MILLIS);  //every 10 minutes get the time
Ticker WeatherChecker(weatherUpdate, MILISPERMIN*15, MILLIS); //every 15 minutes get the weather

time_t currentTime;          //keeps track of the current weather
weatherInfo currentWeather;  //used to keep track of the current weather
bool weatherValid = false;   //whether or not the weatherobject is valid (it will instantiate to noise and needs updating)

//------------------------------- Setup ------------------------------------------\

void setup() {
    //check to see if i need to establish a serial port (not used in release mode)
    #ifdef DEBUGMODE 
    Serial.begin(115200);
    #endif
    
    WiFi.begin(ssid, password);     //begin setting up the wifi connection using data from secrets.h
    int WifiStartTime = millis();                          //get time before we start measuring the timeout.

    while (WiFi.status() != WL_CONNECTED                  //loop around and wait to get online for either a.
    && millis()-WifiStartTime < WIFITIMEOUT) {                //connection or a timeout (currently 5000ms) .
        delay(1000);    
        DEBUG_PRINT(MSG_CONNECTING);                   //keep printing a connecting message to the terminal.
    }                                        //at this point, either timed out or connected. find out which.

    if(WiFi.status() == WL_CONNECTED){                                  //if it's connected to the internet.
        INFO_PRINT(MSG_CONNECTED);                              //just print an info message to serial port. 
    }
    else{   //if not WL_CONNECTED, timed out. This happens sometimes where for some reason it won't connect. 
        ESP.restart();                            //solution so far is to kill the ESP. ugly but functional.
    }

    initDisp();  //setup the display. no return since we don't actually have an ack signal from the display.

    while(!getNTPTime(currentTime)){                   //wait until we can get the time from the NTP server.
        for(int i = 5; i>0; i++){                    //count down from 5 and display a message every second.
            displayUnableToConnectMsg(i);
            delay(1000);
        }
    }

    if(getNewWeather(currentWeather)){      //if we can get the weather straight away, great,
        weatherValid = true;                // do it and set the validity flag.
    }

    NTPChecker.start();                     //setup a ticker that keeps the NTP synchronised.
    WeatherChecker.start();                 //setup a ticker that keeps the Weather synchronised.
    dispBrightness(3);                      //set the brightness to minimum allowed. its already really bright
}

//-------------------------- loop -------------------------------------------

void loop() {    
    getDeviceTime(currentTime);
    if(weatherValid){
        displayTime(currentTime, currentWeather);
    }
    else{
        displayTime(currentTime);
    }
    
  
    if(minute(currentTime)%30 < 15){
        invertDisp(true);
    }
    else{
        invertDisp(false);
    }

    NTPChecker.update();
    WeatherChecker.update();
    delay(200);
}

void NTPUpdate(){
    getNTPTime(currentTime);
}

void weatherUpdate(){
    getNewWeather(currentWeather);
}