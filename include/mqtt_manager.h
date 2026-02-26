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
private:
    const char *broker_address;
    uint16_t broker_port;
    const char *client_id;
    bool is_connected;
    mqtt_callback_t message_callback;

public:
    /**
     * @brief 初始化 MQTT 管理器
     */
    void init();

    /**
     * @brief 連接到 MQTT Broker
     * @param broker MQTT Broker 地址
     * @param port 連接端口
     * @param client_id 客戶端 ID
     * @return 是否連接成功
     */
    bool connect(const char *broker, uint16_t port, const char *client_id);

    /**
     * @brief 訂閱主題
     * @param topic 主題路徑
     * @return 是否訂閱成功
     */
    bool subscribe(const char *topic);

    /**
     * @brief 發布訊息
     * @param topic 主題路徑
     * @param payload 訊息內容
     * @param retain 是否保留訊息
     */
    void publish(const char *topic, const char *payload, bool retain = false);

    /**
     * @brief 檢查 MQTT 連線狀態
     * @return 是否已連接
     */
    bool isConnected();

    /**
     * @brief 設置訊息回調函數
     * @param callback 回調函數指針
     */
    void setCallback(mqtt_callback_t callback);

    /**
     * @brief 處理 MQTT 訊息循環
     */
    void loop();

    /**
     * @brief 斷開 MQTT 連接
     */
    void disconnect();
};

#endif // MQTT_MANAGER_H
