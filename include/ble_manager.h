#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class BLEManager
{
public:
    BLEManager();
    void init();
    void loop();
    void setParameter(const String &key, const String &value);
    String getParameter(const String &key);
    // 可擴充更多藍芽互動API
private:
    BLEServer *pServer;
    BLECharacteristic *pRxCharacteristic;
    BLECharacteristic *pTxCharacteristic;
    void setupServices();
};

#endif // BLE_MANAGER_H
