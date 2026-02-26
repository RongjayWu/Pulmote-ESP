#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

/**
 * @file wifi_manager.h
 * @brief WiFi 管理模組 - 處理 WiFi 連線
 *
 * 功能:
 * - 掃描可用 WiFi 網路
 * - 連接到指定 WiFi
 * - 監控連線狀態
 * - 自動重連機制
 */

class WiFiManager
{
private:
    const char *ssid;
    const char *password;
    bool is_connected;
    unsigned long last_reconnect_time;
    const unsigned long reconnect_interval = 10000; // 10秒

public:
    /**
     * @brief 初始化 WiFi 管理器
     */
    void init();

    /**
     * @brief 連接到 WiFi
     * @param ssid 網路名稱
     * @param password 網路密碼
     * @return 是否連接成功
     */
    bool connect(const char *ssid, const char *password);

    /**
     * @brief 檢查 WiFi 連線狀態
     * @return 是否已連接
     */
    bool isConnected();

    /**
     * @brief 獲取本地 IP 地址
     * @return IP 地址字符串
     */
    String getLocalIP();

    /**
     * @brief 掃描可用 WiFi 網路
     */
    void scanNetworks();

    /**
     * @brief 處理 WiFi 重連邏輯
     */
    void handleReconnect();

    /**
     * @brief 斷開 WiFi 連接
     */
    void disconnect();
};

#endif // WIFI_MANAGER_H
