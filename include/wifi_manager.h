#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

/**
 * @file wifi_manager.h
 * @brief WiFi Manager - Handles WiFi connection and configuration
 *
 * Features:
 * - Scan available WiFi networks
 * - Connect to specified WiFi
 * - Monitor connection status
 * - Auto-reconnect mechanism
 * - AP mode configuration (Web interface)
 * - NVS storage for WiFi config
 */

#define AP_SSID "Pulmote-ESP"
#define AP_PASSWORD "123456789"
#define AP_IP IPAddress(192, 168, 4, 1)
#define AP_GATEWAY IPAddress(192, 168, 4, 1)
#define AP_SUBNET IPAddress(255, 255, 255, 0)

class WiFiManager
{
private:
    char ssid[32];
    char password[64];
    bool is_sta_connected;
    bool is_ap_mode;
    unsigned long last_reconnect_time;
    unsigned long ap_start_time;
    const unsigned long reconnect_interval = 10000; // 10 seconds
    const unsigned long ap_timeout = 600000;        // 10 minutes no config, auto restart

    WebServer *web_server;
    Preferences preferences;

    /**
     * @brief Initialize Web server (AP config UI)
     */
    void initWebServer();

    /**
     * @brief Handle Web root path request (config UI)
     */
    void handleRoot();

    /**
     * @brief Handle Web API scan WiFi
     */
    void handleScanNetworks();

    /**
     * @brief Handle Web API save WiFi config
     */
    void handleSaveConfig();

    /**
     * @brief Handle Web API get current status
     */
    void handleStatus();

    /**
     * @brief Load saved WiFi config from NVS
     */
    bool loadConfigFromNVS();

    /**
     * @brief Save WiFi config to NVS
     */
    void saveConfigToNVS(const char *ssid, const char *password);

public:
    /**
     * @brief Constructor
     */
    WiFiManager();

    /**
     * @brief Initialize WiFi Manager
     */
    void init();

    /**
     * @brief Connect to WiFi
     * @param ssid Network name
     * @param password Network password
     * @return Connection success
     */
    bool connect(const char *ssid, const char *password);

    /**
     * @brief Check WiFi connection status
     * @return Is connected
     */
    bool isConnected();

    /**
     * @brief Check if in AP mode
     * @return Is AP mode
     */
    bool isAPMode();

    /**
     * @brief Get local IP address
     * @return IP address string
     */
    String getLocalIP();

    /**
     * @brief Scan available WiFi networks
     */
    void scanNetworks();

    /**
     * @brief Enter AP mode (for WiFi config)
     */
    void startAPMode();

    /**
     * @brief Exit AP mode
     */
    void stopAPMode();

    /**
     * @brief Handle WiFi reconnect logic and AP timeout
     */
    void handleReconnect();

    /**
     * @brief Update Web server
     */
    void handleWebServer();

    /**
     * @brief Disconnect WiFi
     */
    void disconnect();

    /**
     * @brief Get AP mode SSID
     */
    String getAPSSID();

    /**
     * @brief Clear saved WiFi config
     */
    void clearConfig();

    /**
     * @brief Destructor
     */
    ~WiFiManager();
};

#endif // WIFI_MANAGER_H
