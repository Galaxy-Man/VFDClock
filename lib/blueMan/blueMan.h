#ifndef BLUEMAN_H
#define BLUEMAN_H


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>



//String serverName = "VFDClock";

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


class blueMan{
    public:
    bool setup();

};








#endif
