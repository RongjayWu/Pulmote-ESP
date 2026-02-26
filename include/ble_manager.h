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
    BLEManager(WiFiManager *wifiMgr);
    void init();
    void loop();
    void setParameter(const String &key, const String &value);
    String getParameter(const String &key);

private:
    BLEServer *pServer;
    BLECharacteristic *pRxCharacteristic;
    BLECharacteristic *pTxCharacteristic;
    void setupServices();
    WiFiManager *wifi_manager;
    static void onRxWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param, void *arg);
    void handleRxData(const std::string &data);
};

#endif // BLE_MANAGER_H
