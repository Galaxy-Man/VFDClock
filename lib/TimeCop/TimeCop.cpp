#include <WiFiUdp.h>
#include <time.h>
#include <WiFi.h>
#include <TimeCop.h>
#include <TimeLib.h>
#include <sys/time.h>


WiFiUDP udp;

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

bool getNTPTime(time_t &t){
    timezone utc = {0,0};
    timezone *tz = &utc;
    udp.begin(UDPPORT);
    IPAddress ntpServerIP;
    while(udp.parsePacket()>0);
    DEBUG_PRINT("Finished flushing buffer, about to send request");
    WiFi.hostByName(NTPSERVER, ntpServerIP);
    sendNTPpacket(ntpServerIP);
    int timeout = millis();
    unsigned long unixTime = 0;
    while(millis()-timeout < NTPTIMEOUT){
        int size = udp.parsePacket();
        if(size >= NTP_PACKET_SIZE);{
            //DEBUG_PRINT("RECEIVED NTP PACKET RETURN");
            udp.read(packetBuffer, NTP_PACKET_SIZE);
            unsigned long secsSince1900;
            secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            if(secsSince1900 > 10000){
                DEBUG_PRINT("UNIX TIME DERIVED");
                unixTime = secsSince1900 - 2208988800U;
                DEBUG_PRINT(unixTime);
                t = time_t(unixTime);
                DEBUG_PRINT("FINISHED DECODING TIME");
                timeval epoch = {int(unixTime), 0};
                timeval *tv = &epoch;
                settimeofday(tv, tz);
                return(true);
            }
        }
    }
    
    timeval read;
    gettimeofday(&read, tz);
    t = time_t(read.tv_sec);
    return(false);
}


void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void printTime(time_t &t){
    char s[200];
    DEBUG_PRINT("ABOUT TO FORMAT PRINT");
    sprintf(s, "The time is: %02d:%02d:%02d on %02d/%02d/%04d", hour(t), minute(t), second(t), day(t), month(t), year(t));
    INFO_PRINT(s);
}

void getDeviceTime(time_t &t){
    timezone utc = {0,0};
    timezone *tz = &utc;
    timeval read;
    gettimeofday(&read, tz);
    t = time_t(read.tv_sec);
}