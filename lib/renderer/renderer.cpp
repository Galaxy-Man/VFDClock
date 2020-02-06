#include <renderer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GP9002.h>
#include <time.h>
#include <assets.h>
#include <SPI.h>

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
    char timeMsg[10];

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
    displayTime(time_t &t);
    d.setFont(&RobotoMono_Light6pt7b);


    d.drawBitmap(76, 1, tempurature, 12, 12, 1);
    sprintf(charBuff, "%02dC", w.tNow);
    d.setCursor(90,11);
    d.println(charBuffer);

    d.drawBitmap(76, 14, tmax, 12, 12, 1);
    d.setCursor(90, 24);
    sprintf(charBuffer, "%02dC", w.tMax);
    d.println(charBuffer);

    d.drawBitmap(76, 27, tmin, 12, 12, 1);
    d.setCursor(90, 37);
    sprintf(charBuffer, "%02dC", w.tMin);
    d.println(charBuffer);

    d.drawBitmap(76, 40, sunrise, 12, 12, 1);
    d.setCursor(90, 50);
    sprintf(charBuffer, "%02d:%02d", hour(w.srise), minute(w.srise));
    d.println(charBuffer);

    d.drawBitmap(76, 53, sunset, 12, 12, 1);
    d.setCursor(90, 62);
    sprintf(charBuffer, "%02d:%02d", hour(w.sset), minute(w.sset));
    d.println(charBuffer);

    d.blitWithoutReading();
}




void displayUnableToConnectMsg(int i){
    char s[60];
    d.clearBuffer();
    d.setCursor(3,10);
    d.setFont(&RobotoMono_Light6pt7b);
    sprintf(s, "Unable to get NTP time, trying again in %d", i);
    d.println(s);
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

void invertDisp(bool i){
    d.inverted = i;
}

