#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>

/**
 * @file mqtt_manager.h
 * @brief MQTT 管理模組 - 處理 MQTT 通訊
 *
 * 功能:
 * - 連接到 MQTT Broker
 * - 訂閱控制主題
 * - 發布設備狀態
 * - 接收和處理 MQTT 訊息
 */

typedef void (*mqtt_callback_t)(const char *topic, const char *payload);

class MQTTManager
{
public:
    MQTTManager();
    void init();
    bool subscribe(const char *topic);
    void publish(const char *topic, const char *payload, bool retain = false);
    bool isConnected();
    void setCallback(mqtt_callback_t callback);
    void loop();
    void disconnect();

private:
    const char *broker_address;
    const char *client_id;
    bool is_connected;
    mqtt_callback_t message_callback;
};

#endif // MQTT_MANAGER_H
