#include <renderer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GP9002.h>
#include <time.h>

Adafruit_GP9002 d = Adafruit_GP9002(CLKPIN, MISOPIN, MOSIPIN, CSPIN, DCPIN);

void initDisp(){
    pinMode(33, OUTPUT);
    digitalWrite(33, HIGH);
    d.begin();
    d.clearDisplay();
    d.blitWithoutReading();
}

void displayTime(time_t &t){
    d.clearBuffer();
    d.setCursor(0,0);
    char timeMsg[200];
    sprintf(timeMsg, "%02d:%02d:%02d, %02d/%02d/%04d", hour(t), minute(t), second(t), day(t), month(t), year(t));
    DEBUG_PRINT("sprinted the message");
    //String s = "Time  : " + String(hour(t)) + ":" + String(minute(t)) + ":" + String(second(t));
    d.println(timeMsg);
    d.blitWithoutReading();
}

void displayUnableToConnectMsg(){
    d.clearBuffer();
    d.setCursor(10,10);
    d.println("Unable to get NTP time, trying again in 5");
    d.blitWithoutReading();
}

void refreshDisplay(){
    for(int i = 1; i<64; i++){ 
        d.clearBuffer();
        d.drawRect(64-i, 32-(i/2), i*2, i, 1);
        d.blitWithoutReading();
    }
    
    for(int i = 64; i>0; i--){ 
        d.clearBuffer();
        d.drawRect(64-i, 32-(i/2), i*2, i, 1);
        d.blitWithoutReading();
    }
}

void displayRebooting(){
    d.clearBuffer();
    d.println("trying to get online");
    d.blitWithoutReading();
}