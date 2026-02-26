#include "ble_manager.h"

#include <ArduinoJson.h>
#define SERVICE_UUID "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_RX "12345678-1234-1234-1234-1234567890ac"
#define CHARACTERISTIC_TX "12345678-1234-1234-1234-1234567890ad"

BLEManager::BLEManager(WiFiManager *wifiMgr) : pServer(nullptr), pRxCharacteristic(nullptr), pTxCharacteristic(nullptr), wifi_manager(wifiMgr) {}

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
    pRxCharacteristic->setCallbacks(new class : public BLECharacteristicCallbacks {
        BLEManager *parent;

    public:
        void setParent(BLEManager * p) { parent = p; }
        void onWrite(BLECharacteristic * pChar) override
        {
            std::string value = pChar->getValue();
            if (parent)
                parent->handleRxData(value);
        }
    }());
    ((BLECharacteristicCallbacks *)pRxCharacteristic->getCallbacks())->setParent(this);
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
    // 可擴充：設定參數
}

String BLEManager::getParameter(const String &key)
{
    // 可擴充：取得參數
    return String("");
}

void BLEManager::handleRxData(const std::string &data)
{
    // 預期格式: {"ssid":"xxx","password":"yyy"}
    StaticJsonDocument<128> doc;
    DeserializationError err = deserializeJson(doc, data);
    if (!err && doc.containsKey("ssid") && doc.containsKey("password"))
    {
        String ssid = doc["ssid"].as<String>();
        String pwd = doc["password"].as<String>();
        if (wifi_manager)
        {
            wifi_manager->connect(ssid.c_str(), pwd.c_str());
            wifi_manager->saveConfigToNVS(ssid.c_str(), pwd.c_str());
            if (pTxCharacteristic)
            {
                pTxCharacteristic->setValue("WiFi set");
                pTxCharacteristic->notify();
            }
        }
    }
    else
    {
        if (pTxCharacteristic)
        {
            pTxCharacteristic->setValue("Invalid JSON");
            pTxCharacteristic->notify();
        }
    }
}
