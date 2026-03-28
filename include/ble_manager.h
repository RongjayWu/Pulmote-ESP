#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "wifi_manager.h"

class BLEManager
{
public:
    BLEManager();
    void init();
    void loop();

private:
    BLEServer *pServer;
    BLECharacteristic *pRxCharacteristic;
    BLECharacteristic *pTxCharacteristic;
    void setupServices();
    void handleRxData(const std::string &data);
};

#endif // BLE_MANAGER_H
