#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

/**
 * @file wifi_manager.h
 * @brief WiFi 管理模組 - 處理 WiFi 連線和配置
 *
 * 功能:
 * - 掃描可用 WiFi 網路
 * - 連接到指定 WiFi
 * - 監控連線狀態
 * - 自動重連機制
 * - AP 模式配置（Web 界面）
 * - NVS 存儲 WiFi 配置
 */

#define AP_SSID "Pulmote-ESP"
#define AP_PASSWORD "123456789"
#define AP_IP IPAddress(192, 168, 4, 1)
#define AP_GATEWAY IPAddress(192, 168, 4, 1)
#define AP_SUBNET IPAddress(255, 255, 255, 0)

enum WiFiMode
{
    WIFI_MODE_STA,  // Station 模式 - 連接到其他 WiFi
    WIFI_MODE_AP,   // AP 模式 - ESP32 作為接入點
    WIFI_MODE_APSTA // 同時支持 STA 和 AP
};

class WiFiManager
{
private:
    char ssid[32];
    char password[64];
    bool is_sta_connected;
    bool is_ap_mode;
    unsigned long last_reconnect_time;
    unsigned long ap_start_time;
    const unsigned long reconnect_interval = 10000; // 10秒
    const unsigned long ap_timeout = 600000;        // 10分鐘無配置則自動重啟

    WebServer *web_server;
    Preferences preferences;

    /**
     * @brief 初始化 Web 服務器（AP 配置界面）
     */
    void initWebServer();

    /**
     * @brief 處理 Web 根路徑請求（配置界面）
     */
    void handleRoot();

    /**
     * @brief 處理 Web API 掃描 WiFi
     */
    void handleScanNetworks();

    /**
     * @brief 處理 Web API 保存 WiFi 配置
     */
    void handleSaveConfig();

    /**
     * @brief 處理 Web API 獲取當前狀態
     */
    void handleStatus();

    /**
     * @brief 從 NVS 加載保存的 WiFi 配置
     */
    bool loadConfigFromNVS();

    /**
     * @brief 將 WiFi 配置保存到 NVS
     */
    void saveConfigToNVS(const char *ssid, const char *password);

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
     * @brief 檢查是否在 AP 模式
     * @return 是否在 AP 模式
     */
    bool isAPMode();

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
     * @brief 進入 AP 模式（用於 WiFi 配置）
     */
    void startAPMode();

    /**
     * @brief 退出 AP 模式
     */
    void stopAPMode();

    /**
     * @brief 處理 WiFi 重連邏輯和 AP 超時
     */
    void handleReconnect();

    /**
     * @brief 更新 Web 服務器
     */
    void handleWebServer();

    /**
     * @brief 斷開 WiFi 連接
     */
    void disconnect();

    /**
     * @brief 獲取 AP 模式的 SSID
     */
    String getAPSSID();

    /**
     * @brief 清除保存的 WiFi 配置
     */
    void clearConfig();

    /**
     * @brief 析構函數
     */
    ~WiFiManager();
};

#endif // WIFI_MANAGER_H
