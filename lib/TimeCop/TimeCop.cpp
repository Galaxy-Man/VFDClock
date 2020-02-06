#include <WiFiUdp.h>
#include <time.h>
#include <WiFi.h>
#include <TimeCop.h>
#include <TimeLib.h>
#include <sys/time.h>


WiFiUDP udp;    //udp object. kinda resent using UDP but what can ya do?

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

bool getNTPTime(time_t &t){     //passes the time into the time_t and returns true on success.
    timezone utc = {0,0};   //timezone val. maybe globalise and parametrise this when BT works.
    timezone *tz = &utc;    //uum wut? TODO FIX THIS SHIT
    udp.begin(UDPPORT);     //open UDP connection to port. 
    IPAddress ntpServerIP;  //start an IP object.
    while(udp.parsePacket()>0); //flush UDP Buffer
    DEBUG_PRINT(SENDSTR);   //kinda superfluos IMO
    WiFi.hostByName(NTPSERVER, ntpServerIP);    //DNS Lookup the IP of the NTP server pool
    sendNTPpacket(ntpServerIP);     //send the NTP request to server. 
    int timeout = millis(); //setup a timeout initial value
    unsigned long unixTime = 0;     //and a blank value for the unix time
    while(millis()-timeout < NTPTIMEOUT){       //for the length of the timeout period.
        int size = udp.parsePacket();   //get the size of the UDP buffer.
        if(size >= NTP_PACKET_SIZE);{   //if the size is greater than the size of an NTP packet
            udp.read(packetBuffer, NTP_PACKET_SIZE);    //read in that amount into the packetBuffer
            unsigned long secsSince1900;    //?? maths
            secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];   //more maths?
            if(secsSince1900 > 10000){      //rough and ready check to see if things seem kosher.
                DEBUG_PRINT(ACQSTR);        //print out a message
                unixTime = secsSince1900 - MAGICNUMBER; //maths
                DEBUG_PRINT(unixTime);      //more printing
                t = time_t(unixTime);   //cast the unix number into a time object
                DEBUG_PRINT(DECODESTR); //print a string about decoding.
                timeval epoch = {int(unixTime), 0};     //cast it into a timeval omg
                timeval *tv = &epoch;       //more stuff
                settimeofday(tv, tz);       //set the local time on the RTC
                return(true);       //bye bye
            }
        }
    } 
    timeval read;       //setup a timeval
    gettimeofday(&read, tz);  //get time from RTC. we failed to get NTP, so fallback.
    t = time_t(read.tv_sec);    //cast to time_t for epic compatibility reasons/
    return(false);      //byebye
}


void sendNTPpacket(IPAddress &address){
    memset(packetBuffer, 0, NTP_PACKET_SIZE);    // set all bytes in the buffer to 0
    // Initialize values needed to form NTP request
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}


void printTime(time_t &t){
    char s[50];
    sprintf(s, RENDERSTR, hour(t), minute(t), second(t), day(t), month(t), year(t));
    INFO_PRINT(s);
}


void getDeviceTime(time_t &t){      //get the local time from RTC
    timezone utc = {0,0};       //no bool since i don't think this will fail.
    timezone *tz = &utc;
    timeval read;       //same as above basically.
    gettimeofday(&read, tz);
    t = time_t(read.tv_sec);
}


void renderTime(time_t &t, char* s){
    sprintf(s, RENDERSTR, hour(t), minute(t), second(t), day(t), month(t), year(t));
}