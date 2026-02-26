/**
 * @file mqtt_manager.cpp
 * @brief MQTT 管理模組實現
 *
 * 實現 MQTT 連接、發布和訂閱功能
 */

#include "mqtt_manager.h"

void MQTTManager::init()
{
    is_connected = false;
    message_callback = nullptr;

    Serial.println("[MQTT] MQTT 管理器已初始化");
}

bool MQTTManager::connect(const char *broker, uint16_t port, const char *_client_id)
{
    broker_address = broker;
    broker_port = port;
    client_id = _client_id;

    Serial.printf("[MQTT] 正在連接到 MQTT Broker: %s:%d\n", broker, port);
    Serial.printf("[MQTT] 客戶端 ID: %s\n", client_id);

    // TODO: 使用 PubSubClient 庫進行實際連接
    // 這裡是模擬實現

    is_connected = true;
    Serial.println("✓ MQTT 連接成功");

    return true;
}

bool MQTTManager::subscribe(const char *topic)
{
    if (!is_connected)
    {
        Serial.printf("[MQTT] 錯誤: 未連接到 MQTT Broker，無法訂閱 '%s'\n", topic);
        return false;
    }

    Serial.printf("[MQTT] 訂閱主題: %s\n", topic);

    // TODO: 調用 PubSubClient 的 subscribe() 方法

    return true;
}

void MQTTManager::publish(const char *topic, const char *payload, bool retain)
{
    if (!is_connected)
    {
        Serial.printf("[MQTT] 錯誤: 未連接到 MQTT Broker，無法發佈\n");
        return;
    }

    Serial.printf("[MQTT] 發佈: %s = %s (Retain: %s)\n",
                  topic, payload, retain ? "Yes" : "No");

    // TODO: 調用 PubSubClient 的 publish() 方法
}

bool MQTTManager::isConnected()
{
    return is_connected;
}

void MQTTManager::setCallback(mqtt_callback_t callback)
{
    message_callback = callback;
    Serial.println("[MQTT] 訊息回調函數已設置");
}

void MQTTManager::loop()
{
    if (!is_connected)
    {
        return;
    }

    // TODO: 調用 PubSubClient 的 loop() 方法
    // client.loop();
}

void MQTTManager::disconnect()
{
    if (is_connected)
    {
        // TODO: 調用 PubSubClient 的 disconnect() 方法
        is_connected = false;
        Serial.println("[MQTT] MQTT 已斷開");
    }
}
