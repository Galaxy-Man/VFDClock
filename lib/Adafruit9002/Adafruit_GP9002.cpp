#if (defined(__AVR__))
#include <avr\pgmspace.h>
#include <util/delay.h>
#else
#include <pgmspace.h>
#define _delay_ms(ms) delayMicroseconds((ms) * 1000)
#endif //since i'm working on an ESP

#define SLOWDOWN 1
#include <stdlib.h>

#include "Adafruit_GFX.h"
#include "Adafruit_GP9002.h"
#include "glcdfont.c"
#include "img.h" 

Adafruit_GP9002::Adafruit_GP9002(int8_t SCLK, int8_t MISO, int8_t MOSI, 
    int8_t CS, int8_t DC) : Adafruit_GFX(128, 64) {
  _sclk = SCLK;
  _miso = MISO;
  _mosi = MOSI;
  _cs = CS;
  _dc = DC;
  hwSPI = false;
}

Adafruit_GP9002::Adafruit_GP9002(int8_t CS, int8_t DC) :
    Adafruit_GFX(128, 64) {
  _sclk = -1;
  _miso = -1;
  _mosi = -1;
  _cs = CS;
  _dc = DC;
  hwSPI = true;
}

void Adafruit_GP9002::begin(void) {
  // set pin directions
  pinMode(_dc, OUTPUT);
  pinMode(_cs, OUTPUT);

  if (! hwSPI) {
    pinMode(_mosi, OUTPUT);
    pinMode(_miso, INPUT);
    pinMode(_sclk, OUTPUT);
    
    clkport     = (volatile uint8_t*)portOutputRegister(digitalPinToPort(_sclk));
    clkpinmask  = digitalPinToBitMask(_sclk);
    mosiport    = (volatile uint8_t*)portOutputRegister(digitalPinToPort(_mosi));
    mosipinmask = digitalPinToBitMask(_mosi);
    misopin = (volatile uint8_t*)portInputRegister(digitalPinToPort(_miso));
    misopinmask = digitalPinToBitMask(_miso);
  } else {
    // SPI.begin();
    // SPI.setClockDivider(SPI_CLOCK_DIV4);
    // SPI.setBitOrder(MSBFIRST);
    // SPI.setDataMode(SPI_MODE0);
  }
  csport    = (volatile uint8_t*)portOutputRegister(digitalPinToPort(_cs));
  cspinmask = digitalPinToBitMask(_cs);
  dcport    = (volatile uint8_t*)portOutputRegister(digitalPinToPort(_dc));
  dcpinmask = digitalPinToBitMask(_dc);

  command(GP9002_DISPLAY);
  dataWrite(GP9002_DISPLAY_MONOCHROME);
  command(GP9002_LOWERADDR1);
  dataWrite(0x0);
  command(GP9002_HIGHERADDR1);
  dataWrite(0x0);
  command(GP9002_LOWERADDR2);
  dataWrite(0x0);
  command(GP9002_HIGHERADDR2);
  dataWrite(0x4);
  command(GP9002_OR);
  command(GP9002_CLEARSCREEN);
  command(GP9002_DISPLAY1ON);

  // hold the address so we can read and then write
  command(GP9002_ADDRHELD);
}



// the most basic function, set a single pixel
void Adafruit_GP9002::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;
    
  uint16_t addr = 0;
  addr = x*8;
  y = 63 - y;
  addr += y/8;

  uint8_t p;
  p = bitmap[addr];

  if (color)
    p |= (1 << (7-(y % 8)));
  else
    p &= ~(1 << (7-(y % 8)));

    bitmap[addr] = p;
}

void Adafruit_GP9002::blitWithoutReading(){
  command(GP9002_ADDRINCR);
  command(GP9002_ADDRL);
  dataWrite(0x00);
  command(GP9002_ADDRH);
  dataWrite(0x00);
  command(GP9002_DATAWRITE);
  dataWrite(bitmap, (imageHeight*imageWidth/8));
}

inline void Adafruit_GP9002::dataWrite(uint8_t *d, int len) {
    *dcport &= ~dcpinmask;
    *csport &= ~cspinmask;
    for (int i=0; i<len; i++){
        if(inverted){
            slowSPIwrite(~d[i]);
        }
        else{
            slowSPIwrite(d[i]);
        }
        delayMicroseconds(SLOWDOWN); // should be 600ns actually
    }
    *csport |= cspinmask;
    delayMicroseconds(SLOWDOWN); // should be 600ns actually
}
  

void Adafruit_GP9002::invert(void) {
    inverted = !inverted;
    blitWithoutReading();
}

void Adafruit_GP9002::slowSPIwrite(uint8_t d) {
 for (uint8_t i=0; i<8; i++) {
   digitalWrite(_sclk, LOW);
   if (d & _BV(i)) {
     digitalWrite(_mosi, HIGH);
   } else {
     digitalWrite(_mosi, LOW);
   }
   delayMicroseconds(SLOWDOWN);
   digitalWrite(_sclk, HIGH);
   
 }
}


uint8_t Adafruit_GP9002::slowSPIread(void) {
 uint8_t reply = 0;
 for (uint8_t i=0; i<8; i++) {
   digitalWrite(_sclk, LOW);

   digitalWrite(_sclk, HIGH);
   delayMicroseconds(10*SLOWDOWN);
   if (digitalRead(_miso)) 
     reply |= _BV(i);
 }
 return reply;
}


void Adafruit_GP9002::command(uint8_t d) { 
  *dcport |= dcpinmask;
  *csport &= ~cspinmask;

  slowSPIwrite(d);
  *csport |= cspinmask;
  delayMicroseconds(SLOWDOWN); // should be 400ns actually
}

inline void Adafruit_GP9002::dataWrite(uint8_t d) {
  *dcport &= ~dcpinmask;
  *csport &= ~cspinmask;
  delayMicroseconds(SLOWDOWN);
  slowSPIwrite(d);

  *csport |= cspinmask;
  delayMicroseconds(SLOWDOWN); // should be 600ns actually
}
inline uint8_t Adafruit_GP9002::dataRead() {
  uint8_t r;

  *dcport &= ~dcpinmask;
  *csport &= ~cspinmask; 
  delayMicroseconds(SLOWDOWN);
  r = slowSPIread();

  *csport |= cspinmask;
  delayMicroseconds(SLOWDOWN);
  
 return r;
}





void Adafruit_GP9002::setBrightness(uint8_t val) {
    command(GP9002_BRIGHT);
    switch(val){
        case 0:
            dataWrite(0xFF);
            break;
        
        case 1:
            dataWrite(0xFF);
            break;
        
        case 2:
            dataWrite(0xFF);
            break;
        
        case 3: 
            dataWrite(0x2A);
            break;
        
        case 4:
            dataWrite(0x24);
            break;

        case 5: 
            dataWrite(0x1E);
            break;
        
        case 6:
            dataWrite(0x18);
            break;

        case 7: 
            dataWrite(0x12);
            break;
        
        case 8:
            dataWrite(0x0C);
            break;
        
        case 9: 
            dataWrite(0x06);
            break;
        
        case 10:
            dataWrite(0x00);
            break;
    }
}

// clear everything
void Adafruit_GP9002::clearDisplay(void) {
  command(GP9002_CLEARSCREEN);
}

void Adafruit_GP9002::clearBuffer(void){
    for(int i = 0; i < sizeof(bitmap); i++){
      bitmap[i] = 0x00;
  }
}

void Adafruit_GP9002::displayOff(void) {
  command(GP9002_DISPLAYSOFF); 
}
void Adafruit_GP9002::displayOn(void) {
   command(GP9002_DISPLAY1ON);
}




#define imageWidth 128
#define imageHeight 64

