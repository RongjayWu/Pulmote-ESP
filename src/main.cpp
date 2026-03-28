// Pulmote ESP32 Main Program Skeleton
#include "ble_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "ir_manager.h"
#include <Preferences.h>

void clearWifiConfig() // 清除 Preferences 中的 WiFi SSID 與密碼
{
    Preferences preferences;
    preferences.begin("wifi_config", false); // 寫入模式
    preferences.remove("ssid");              // 移除 SSID
    preferences.remove("password");          // 移除密碼
    preferences.end();
}

WiFiManager wifiManager;
BLEManager bleManager;
MQTTManager mqttManager;
IRManager irManager;
uint16_t ir_rx_pin = 15; // Example IR receive pin
uint16_t ir_tx_pin = 4;  // Example IR send pin
uint16_t dev_status_pin = 2;

void setup()
{
    // Do not clear WiFi credentials on every boot
    // clearWifiConfig(); // 這行會在每次啟動時清除 WiFi 設定，請根據需要決定是否保留
    Serial.begin(115200);
    delay(50);
    Serial.println("Main: startup - Serial initialized");
    pinMode(dev_status_pin, OUTPUT); // 初始化 LED 腳位
    wifiManager.init(dev_status_pin);
    bleManager.init();
    mqttManager.init();
    irManager.init(ir_rx_pin, ir_tx_pin, dev_status_pin);
    // ...其他初始化流程...
}

void loop()
{
    wifiManager.loop();
    // ...其他主程式邏輯...
}
