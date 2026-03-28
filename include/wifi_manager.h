#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

class WiFiManager
{
public:
    WiFiManager();                  // 建構子
    ~WiFiManager();                 // 解構子，釋放 Preferences / WebServer
    void init(uint16_t status_pin); // WiFi 初始化流程
    void startWebServer();          // 處理 Web Server 的 Client 請求
    void stopWebServer();
    void startAPMode(); // 啟動 AP 模式
    void stopAPMode();
    bool isAPActive();
    void loop();             // 主循環處理
    void statusPinControl(); // 控制狀態指示燈
    void handleConnect();    // 處理 WiFi 連線事件

private:
    uint16_t dev_status_pin;       // 狀態指示燈腳位
    WebServer *webServer;          // Web Server 物件指標
    Preferences preferences;       // 用於存儲 WiFi 資訊的 Preferences
    String lastAttemptSsid;        // 上次嘗試連接的 SSID
    String lastAttemptPassword;    // 上次嘗試連接的密碼
    unsigned long lastBlinkMillis; // 上次切換 LED 的時間 (ms)
    bool ledState;                 // LED 當前狀態 (true = HIGH)
    unsigned int blinkIntervalMs;  // 閃爍間隔 (毫秒)
};

#endif