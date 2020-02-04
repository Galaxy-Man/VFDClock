#include <blueMan.h>

#include <WString.h>


bool blueMan::setup(){
    BLEDevice::init(/*need to figure that out*/);
    BLEServer *BlueServer = BLEDevice::createServer(SERVICE_UUID);
    BLEService *BlueService = blueServer->createService(SERVICE_UUID);
    BLECharacteristic *AlarmCharacteristic =    /*broken line*/
    BlueService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE);
    AlarmCharacteristic->setValue("07:30");
    BLEAdvertising *BlueAdvertising = BlueServer->getAdvertising();
    BlueAdvertising->start();


    return(true);
}