#include "ble_manager.h"

#define SERVICE_UUID "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_RX "12345678-1234-1234-1234-1234567890ac"
#define CHARACTERISTIC_TX "12345678-1234-1234-1234-1234567890ad"

BLEManager::BLEManager() : pServer(nullptr), pRxCharacteristic(nullptr), pTxCharacteristic(nullptr) {}

void BLEManager::init()
{
    BLEDevice::init("Pulmote-ESP-BLE");
    pServer = BLEDevice::createServer();
    setupServices();
}

void BLEManager::setupServices()
{
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_RX,
        BLECharacteristic::PROPERTY_WRITE);
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_TX,
        BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());
    pService->start();
    BLEDevice::startAdvertising();
}

void BLEManager::loop()
{
    // 可擴充：處理藍芽事件
}

void BLEManager::setParameter(const String &key, const String &value)
{
    // 預留：設定參數
}

String BLEManager::getParameter(const String &key)
{
    // 預留：取得參數
    return String("");
}
