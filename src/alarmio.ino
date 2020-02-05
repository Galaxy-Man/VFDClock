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
volatile bool buttonFlag = false;                       //is set on interrupt and cleared when a task picks up on it.

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
    if(weatherValid){                             //if the weather was collected successfully at least once.
        displayTime(currentTime, currentWeather);                              //draw both time and weather. 
    }
    else{
        displayTime(currentTime);  //if the weather has not been confirmed at least once, just display time.
    }
    
  
    if(hour(currentTime)>19 || hour(currentTime)<7){                  //if the hour is between 7PM and 7AM.
        invertDisp(false);                                                        //dont invert the screen.
    }                 //NB this is done to prevent burn-in by inverting the static elements every 12 hours.
    else{
        invertDisp(true);                       //if the hour is 7AM to 7PM, then set the display inverted.
    }          //this is done so the higher brightness mode happens during the day, and lower at nighttime. 

    if(hour(currentTime) == 7 && minute(currentTime) == 50 &&    /*broken line.*/
       second(currentTime) <5 && alarmBeeper.state() != RUNNING){      //this condition triggers the alarm.
        alarmBeeper.start();                                   //start the alarmBeeper (this beeps loudly).
        secondTicker.stop();           //stop the secondTicker temporarily as it interferes with the alarm.
        INFO_PRINT("ALARM");                                                                  //Info print.
    }

    NTPChecker.update();        //updates all the Tickers.
    WeatherChecker.update();
    secondTicker.update();
    alarmBeeper.update();


    if(buttonFlag){              //if the button has recently been pressed, handle it and disable the flag.
            if(alarmBeeper.state() == RUNNING){  //if the alarm is running (this logic might get reworked).
            alarmBeeper.stop();                                                    //stop the alarm beeper.
            secondTicker.start();                                        //start the second ticker as well.
            INFO_PRINT("Alarm stopped due to button press");                                  //info print.
        }

        buttonFlag = false;                                                       //finally reset the flag.
    }
}


void tickBuzzer(){                            //this simply makes the buzzer emit a quiet tick noise once.
    ledcDetachPin(BUZZERPIN);             //this is needed since we're not using PWM we have to detach it.
    pinMode(BUZZERPIN, OUTPUT);                           //once that's done, set the buzzer as an output.
    digitalWrite(BUZZERPIN, HIGH);                                              //set the buzzer out high.
    delayMicroseconds(6);                                                             //short short delay.
    digitalWrite(BUZZERPIN, LOW);                                                //set the buzzer out low.
}


void NTPUpdate(){             //this calls the NTP service. done in a wrapper function to enable checking.
    getNTPTime(currentTime);                                     //get the NTP Time. TODO: error handling.
}


void weatherUpdate(){             //this calls the weather service. again in a wrapper for error handling.
    getNewWeather(currentWeather);                                     //get weather TODO: error handling.
}


void alarmBeep(){    //this function uses a static var. every time it's called it sets the tone on or off.
    ledcAttachPin(BUZZERPIN, 2);               //need to reattach pin to PWM incase seconds has run since.
    static bool isBeeping = false;                  //static var to track if the alarm is in a tone or no.
    if(isBeeping){                                                                     //if we're beeping.
        isBeeping = false;                                                              //now we won't be. 
        ledcWrite(2, 0);                                                        //then set the buzzer off.
    }
    else{
        isBeeping = true;                         //if we're currently not beeping, we will start beeping.
        ledcWrite(2,128);                          //set the duty cycle to half (needed for a 50/50 duty).
    }     
}


void buttonInterrupt(){    //irupt handler for the button. Needs reworking to fix debounce non-blockingly.
    if(buttonFlag){
        return;
    }
    DEBUG_PRINT(BUTTON_MSG);                                                                //debug print.
    int start = millis();                                   //set the start time for the debounce counter.
    while(millis() - start < 100){        //temporary debounce logic just waits 100ms and refreshes alarm.
        alarmBeeper.update();                                                    //refreshing alarm timer.
    }
    if(digitalRead(BUTTONPIN)){                      //if the button pin is still held, then set the flag.
        buttonFlag = true;
    }           //annoyingly, you can't delay for very long, it timeouts a watchdog somewhere in hardware.
}