//--------------------------------- Includes --------------------------------------------------------------.

#include <definitions.h>
#include <WiFi.h>
#include <time.h>
#include <TimeCop.h>
#include <renderer.h>
#include "Ticker.h"
#include <Weatherman.h>
#include <Secrets.h>
#include <Strings.h>

//------------------------------- Global vars -------------------------------------------------------------.

Ticker NTPChecker(NTPUpdate, MILISPERMIN*10, 0, MILLIS);                    //every 10 minutes get the time.
Ticker WeatherChecker(weatherUpdate, MILISPERMIN*15, 0, MILLIS);         //every 15 minutes get the weather.
Ticker secondTicker(tickBuzzer, 1000, 0, MILLIS);                        //every second to make buzzer tick.
Ticker alarmBeeper(alarmBeep, BEEPLENGTH,0,  MILLIS);         //toggles the alarm beep every BEEPLENGTH.
Ticker screenDrawer(handleScreen, REFRESHRATE, 0, MILLIS);

time_t currentTime;                                                       //keeps track of the current time.
weatherInfo currentWeather;                                     //used to keep track of the current weather.
bool weatherValid = false;   //whether or not the weatherobj is valid (not instantiated and needs updating).
bool buttonFlag = false;                       //is set on interrupt and cleared when a task picks up on it.

//---------------------------------- Setup ----------------------------------------------------------------.

void setup() {
                             //check to see if i need to establish a serial port (not used in release mode).
    #ifdef DEBUGMODE 
    Serial.begin(115200);
    #endif

    pinMode(BUTTONPIN, INPUT_PULLDOWN);                                     //button pin setup to pull down.
    attachInterrupt(BUTTONPIN, buttonInterrupt, RISING);       //setup a callback for when the button rises.
    ledcSetup(2, BEEPFREQ, 8);                                            //set ch 2 to run at BEEPFREQ.
    
    
    WiFi.begin(ssid, password);            //begin setting up the wifi connection using data from secrets.h.
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
        for(int i = 5; i>0; i--){                    //count down from 5 and display a message every second.
            displayUnableToConnectMsg(i);           
            delay(1000);
        }
    }

    if(getNewWeather(currentWeather)){                     //if we can get the weather straight away, great.
        weatherValid = true;                // do it and set the validity flag(TODO: Neater way to do this).
    }

    NTPChecker.start();                                    //setup a ticker that keeps the NTP synchronised.
    WeatherChecker.start();                            //setup a ticker that keeps the Weather synchronised.
    secondTicker.start();                                           //ticks once a second to make the noise.
    dispBrightness(3);                   //set the brightness to minimum allowed. its already really bright.
  
}
//---------------------------------------------- loop ------------------------------------------------------

void loop() {                                 //main loop. my aim in this code is to minimise this function. 
    getDeviceTime(currentTime);       //get the local time from the RTC. This will eventually need handling.
    if(weatherValid){
        displayTime(currentTime, currentWeather);
    }
    else{
        displayTime(currentTime);
    }
    
  
    if(hour(currentTime)>19 || hour(currentTime)<7){
        invertDisp(false);
    }
    else{
        invertDisp(true);
    }

    if(hour(currentTime) == 7 && minute(currentTime) == 50 && second(currentTime) <5 && alarmBeeper.state() != RUNNING){
        alarmBeeper.start();
        secondTicker.stop();
        INFO_PRINT("ALARM");
    }

    NTPChecker.update();
    WeatherChecker.update();
    secondTicker.update();
    alarmBeeper.update();

    if(buttonFlag){
        if(alarmBeeper.state() == RUNNING){
            alarmBeeper.stop();
            secondTicker.start();
            INFO_PRINT("Alarm stopped due to button press");
        }

        buttonFlag = false;
    }
}


void tickBuzzer(){
    ledcDetachPin(BUZZERPIN);
    pinMode(BUZZERPIN, OUTPUT);
    digitalWrite(BUZZERPIN, HIGH);
    delayMicroseconds(6);
    digitalWrite(BUZZERPIN, LOW);
}


void NTPUpdate(){
    getNTPTime(currentTime);
}


void weatherUpdate(){
    getNewWeather(currentWeather);
}


void alarmBeep(){
    ledcAttachPin(BUZZERPIN, 2);
    static bool isBeeping = false;
    if(isBeeping){
        isBeeping = false;
        ledcWrite(2, 0);
    }
    else{
        isBeeping = true;
        ledcWrite(2,128);
    }     
}


void buttonInterrupt(){
    DEBUG_PRINT(BUTTON_MSG);
    int start = millis();
    while(millis() - start < 100){
        alarmBeeper.update();
    }
    if(digitalRead(BUTTONPIN)){
        buttonFlag = true;
    }
}

