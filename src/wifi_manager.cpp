/**
 * @file wifi_manager.cpp
 * @brief WiFi 管理模組實現
 *
 * 實現 WiFi 連接和狀態管理功能
 */

#include "wifi_manager.h"

void WiFiManager::init()
{
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    is_connected = false;
    last_reconnect_time = 0;

    Serial.println("[WiFi] WiFi 管理器已初始化");
    Serial.printf("[WiFi] WiFi 模式: STA (Station)\n");
}

bool WiFiManager::connect(const char *_ssid, const char *_password)
{
    ssid = _ssid;
    password = _password;

    Serial.printf("[WiFi] 正在連接到 '%s'...\n", ssid);

    WiFi.begin(ssid, password);

    // 等待連接（最多 20 秒）
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        is_connected = true;
        Serial.println("\n✓ WiFi 連接成功");
        Serial.printf("[WiFi] 本地 IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WiFi] 信號強度: %d dBm\n", WiFi.RSSI());
        return true;
    }
    else
    {
        is_connected = false;
        Serial.println("\n✗ WiFi 連接失敗");
        Serial.printf("[WiFi] WiFi 狀態: %d\n", WiFi.status());
        return false;
    }
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getLocalIP()
{
    if (isConnected())
    {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

void WiFiManager::scanNetworks()
{
    Serial.println("[WiFi] 掃描可用網路...");

    int n = WiFi.scanNetworks();

    if (n == 0)
    {
        Serial.println("[WiFi] 未找到任何網路");
        return;
    }

    Serial.printf("[WiFi] 找到 %d 個網路:\n", n);

    for (int i = 0; i < n; ++i)
    {
        Serial.printf("  %d. SSID: %s | 信號強度: %d dBm | 加密: %d\n",
                      i + 1,
                      WiFi.SSID(i).c_str(),
                      WiFi.RSSI(i),
                      WiFi.encryptionType(i));
    }
}

void WiFiManager::handleReconnect()
{
    if (!isConnected())
    {
        unsigned long current_time = millis();

        if (current_time - last_reconnect_time >= reconnect_interval)
        {
            last_reconnect_time = current_time;

            Serial.println("[WiFi] 正在重新連接...");
            WiFi.reconnect();
        }
    }
    else
    {
        is_connected = true;
    }
}

void WiFiManager::disconnect()
{
    WiFi.disconnect(true); // 斷開連接並關閉 WiFi
    is_connected = false;
    Serial.println("[WiFi] WiFi 已斷開");
}
