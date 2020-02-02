#include <renderer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GP9002.h>
#include <time.h>
#include <assets.h>

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
    char wBuffer[20];
    
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

    d.setFont(&RobotoMono_Light6pt7b);





    d.drawBitmap(76, 1, temp, 12, 12, 1);
    sprintf(wBuffer, "%02dC", w.currentTemp);
    d.setCursor(90,11);
    d.println(wBuffer);


    d.drawBitmap(76, 14, tmax, 12, 12, 1);
    d.setCursor(90, 24);
    sprintf(wBuffer, "%02dC", w.tempMax);
    d.println(wBuffer);

    d.drawBitmap(76, 27, tmin, 12, 12, 1);
    d.setCursor(90, 37);
    sprintf(wBuffer, "%02dC", w.tempMin);
    d.println(wBuffer);

    d.drawBitmap(76, 40, sunrise, 12, 12, 1);
    d.setCursor(90, 50);
    sprintf(wBuffer, "%02d:%02d", hour(w.sunrise), minute(w.sunrise));
    d.println(wBuffer);

    d.drawBitmap(76, 53, sunset, 12, 12, 1);
    d.setCursor(90, 62);
    sprintf(wBuffer, "%02d:%02d", hour(w.sunset), minute(w.sunset));
    d.println(wBuffer);

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

void invertDisp(bool i){
    if(i){d.inverted = true;}
    else{d.inverted = false;}
}

