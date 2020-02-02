#ifndef DEFS_H
#define DEFS_H


#define WIFITIMEOUT 5000        //default time the ESP tries to get online before restarting

#define DEBUGMODE               //set this to DEBUG to get lots of messages on the terminal

#ifndef DEBUGMODE
#define DEBUG_PRINT(X) 
#define INFO_PRINT(X)
#else
#define DEBUG_PRINT(X) Serial.print("DEBUG: ");Serial.println(X);
#define INFO_PRINT(X) Serial.print("INFO: ");Serial.println(X);
#endif


void NTPUpdate();       //forward-declaring the two getter functions because i want my Tickers
void weatherUpdate();   //declared towards the top of the file. 
void buttonInterrupt(); //handles button pressing
void tickBuzzer(); //ticks the buzzer like a clock
void alarmBeep();  //called every BEEPLENGTH to toggle the alarm noise



#define BUTTONPIN 14 //for the button on the device.
#define BUZZERPIN 26 //For the buzzer.
#define MILISPERMIN 1000*60     //convenience. maths hard.

#define BEEPFREQ 4000      //Hz
#define BEEPLENGTH 200    //miliseconds

#endif