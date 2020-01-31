#include <renderer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GP9002.h>
#include <time.h>

#include "RobotoMono_Light6pt7b.h"
#include "RobotoMono_Bold17pt7b.h"

Adafruit_GP9002 d = Adafruit_GP9002(CLKPIN, MISOPIN, MOSIPIN, CSPIN, DCPIN);

void initDisp(){
    pinMode(33, OUTPUT);
    digitalWrite(33, HIGH);
    d.begin();
    d.setFont(&RobotoMono_Light6pt7b);
    d.clearDisplay();
    d.blitWithoutReading();
}

void displayTime(time_t &t){
    d.clearBuffer();
    char timeMsg[15];

    d.setFont(&RobotoMono_Bold17pt7b);
    d.setCursor(3,24);
    sprintf(timeMsg, "%02d", hour(t));
    d.print(timeMsg);
    
    d.setCursor(3,51);
    sprintf(timeMsg, "%02d", minute(t));
    d.print(timeMsg);
    
    d.setCursor(3,62);
    d.setFont(&RobotoMono_Light6pt7b);
    sprintf(timeMsg, "%02d/%02d/%02d", day(t), month(t), year(t)-2000);
    d.print(timeMsg);
  
    d.fillRect(0, 61 - second(t), 2, second(t), 1);

    d.blitWithoutReading();
}

void displayTime(time_t &t, weatherInfo &w){
    d.clearBuffer();
    char timeMsg[15];
    char temp[20];
    
    d.setFont(&RobotoMono_Bold17pt7b);
    d.setCursor(3,24);
    sprintf(timeMsg, "%02d", hour(t));
    d.print(timeMsg);
    
    d.setCursor(3,51);
    sprintf(timeMsg, "%02d", minute(t));
    d.print(timeMsg);
    
    d.setCursor(3,62);
    d.setFont(&RobotoMono_Light6pt7b);
    sprintf(timeMsg, "%02d/%02d/%02d", day(t), month(t), year(t)-2000);
    d.print(timeMsg);
  
    d.fillRect(0, 61 - second(t), 2, second(t), 1);

    d.drawBitmap(90, 0, thermometer, 8, 12, 1);
    sprintf(temp, "%02dC", w.currentTemp);
    d.setCursor(100,10);
    d.setFont(&RobotoMono_Light6pt7b);
    d.println(temp);
    d.blitWithoutReading(); 


    d.blitWithoutReading();
}

void displayUnableToConnectMsg(){
    d.clearBuffer();
    d.setCursor(3,10);
    d.setFont(&RobotoMono_Light6pt7b);
    d.println("Unable to get NTP time, trying again in 5");
    d.blitWithoutReading();
}

void refreshDisplay(){
    for(int i = 0; i<127; i+=3){ 
        d.clearBuffer();
        d.fillRect(i, 0, 5, 63,1);
        d.blitWithoutReading();
        delayMicroseconds(100);
    }
    d.clearBuffer();
    d.blitWithoutReading();
}

void displayRebooting(){
    d.clearBuffer();
    d.setCursor(3,10);
    d.setFont(&RobotoMono_Light6pt7b);
    d.println("trying to get online");
    d.blitWithoutReading();
}

void dispBrightness(int v){
    d.setBrightness(v);
}

void invertDisp(){
    d.invert();
}

