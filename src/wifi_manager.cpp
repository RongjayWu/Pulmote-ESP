/**
 * @file wifi_manager.cpp
 * @brief WiFi Manager Implementation
 */

#include "wifi_manager.h"
#include "html_config.h"

WiFiManager::WiFiManager() : web_server(nullptr), is_sta_connected(false), is_ap_mode(false), last_reconnect_time(0), ap_start_time(0)
{
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
}

void WiFiManager::init()
{
    Serial.println("[WiFi] Initializing...");

    if (!preferences.begin("wifi_config", false))
    {
        Serial.println("[WiFi] NVS init failed");
    }

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    is_sta_connected = false;
    is_ap_mode = false;
    last_reconnect_time = 0;

    if (loadConfigFromNVS())
    {
        Serial.printf("[WiFi] Loaded SSID: %s\n", ssid);
        if (connect(ssid, password))
        {
            is_sta_connected = true;
            Serial.println("[WiFi] Connected!");
        }
        else
        {
            Serial.println("[WiFi] Failed, AP mode");
            startAPMode();
        }
    }
    else
    {
        Serial.println("[WiFi] No config, AP mode");
        startAPMode();
    }

    Serial.println("[WiFi] Init done");
}

bool WiFiManager::loadConfigFromNVS()
{
    if (!preferences.isKey("ssid"))
        return false;

    String saved_ssid = preferences.getString("ssid", "");
    String saved_password = preferences.getString("password", "");

    if (saved_ssid.length() > 0)
    {
        strncpy(ssid, saved_ssid.c_str(), sizeof(ssid) - 1);
        strncpy(password, saved_password.c_str(), sizeof(password) - 1);
        return true;
    }

    return false;
}

void WiFiManager::saveConfigToNVS(const char *_ssid, const char *_password)
{
    preferences.putString("ssid", _ssid);
    preferences.putString("password", _password);
    preferences.putULong("saved_time", millis());
    Serial.printf("[WiFi] Saved: %s\n", _ssid);
}

void WiFiManager::initWebServer()
{
    if (web_server == nullptr)
    {
        web_server = new WebServer(80);

        web_server->on("/", HTTP_GET, [this]()
                       { handleRoot(); });
        web_server->on("/api/scan", HTTP_GET, [this]()
                       { handleScanNetworks(); });
        web_server->on("/api/save", HTTP_POST, [this]()
                       { handleSaveConfig(); });
        web_server->on("/api/status", HTTP_GET, [this]()
                       { handleStatus(); });

        web_server->begin();
        Serial.println("[WiFi] Web server at http://192.168.4.1");
    }
}

void WiFiManager::handleRoot()
{
    web_server->send(200, "text/html; charset=utf-8", WIFI_CONFIG_HTML);
}

void WiFiManager::handleScanNetworks()
{
    Serial.println("[WiFi] Scan request");

    int n = WiFi.scanNetworks();
    String json = "[";

    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
            json += ",";

        json += "{\"ssid\":\"";
        json += WiFi.SSID(i);
        json += "\",\"rssi\":";
        json += WiFi.RSSI(i);
        json += "}";
    }

    json += "]";
    web_server->send(200, "application/json", json);
}

void WiFiManager::handleSaveConfig()
{
    if (!web_server->hasArg("plain"))
    {
        web_server->send(400, "application/json", "{\"success\":false}");
        return;
    }

    String body = web_server->arg("plain");

    int ssid_start = body.indexOf("\"ssid\":\"") + 8;
    int ssid_end = body.indexOf("\"", ssid_start);
    String new_ssid = body.substring(ssid_start, ssid_end);

    int pass_start = body.indexOf("\"password\":\"") + 12;
    int pass_end = body.indexOf("\"", pass_start);
    String new_password = body.substring(pass_start, pass_end);

    if (new_ssid.length() == 0)
    {
        web_server->send(400, "application/json", "{\"success\":false,\"message\":\"Empty\"}");
        return;
    }

    saveConfigToNVS(new_ssid.c_str(), new_password.c_str());
    strncpy(ssid, new_ssid.c_str(), sizeof(ssid) - 1);
    strncpy(password, new_password.c_str(), sizeof(password) - 1);

    web_server->send(200, "application/json", "{\"success\":true}");

    delay(5000);
    ESP.restart();
}

void WiFiManager::handleStatus()
{
    String json = "{";
    json += "\"sta_connected\":" + String(is_sta_connected ? "true" : "false") + ",";
    json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\"";
    json += "}";

    web_server->send(200, "application/json", json);
}

void WiFiManager::startAPMode()
{
    Serial.println("[WiFi] Starting AP...");

    is_ap_mode = true;
    ap_start_time = millis();

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
    bool ap_ok = WiFi.softAP(AP_SSID, AP_PASSWORD);

    if (ap_ok)
    {
        Serial.printf("[WiFi] SSID: %s\n", AP_SSID);
        Serial.printf("[WiFi] IP: %s\n", WiFi.softAPIP().toString().c_str());
        initWebServer();
        // 啟動 DNS Server，將所有域名導向本機
        dns_server.start(53, "*", AP_IP);
    }
    else
    {
        Serial.println("[WiFi] AP start failed");
    }
}

void WiFiManager::stopAPMode()
{
    if (is_ap_mode)
    {
        WiFi.mode(WIFI_STA);
        is_ap_mode = false;

        if (web_server != nullptr)
        {
            web_server->stop();
            delete web_server;
            web_server = nullptr;
        }

        Serial.println("[WiFi] AP stopped");
    }
}

bool WiFiManager::connect(const char *_ssid, const char *_password)
{
    Serial.printf("[WiFi] Connecting to %s...\n", _ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        is_sta_connected = true;
        strncpy(ssid, _ssid, sizeof(ssid) - 1);
        strncpy(password, _password, sizeof(password) - 1);

        Serial.println(" OK");
        Serial.printf("[WiFi] IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }
    else
    {
        is_sta_connected = false;
        Serial.println(" FAIL");
        return false;
    }
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::isAPMode()
{
    return is_ap_mode;
}

String WiFiManager::getLocalIP()
{
    if (is_sta_connected && WiFi.status() == WL_CONNECTED)
    {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

void WiFiManager::scanNetworks()
{
    Serial.println("[WiFi] Scanning...");
    int n = WiFi.scanNetworks();

    if (n == 0)
    {
        Serial.println("[WiFi] No networks");
        return;
    }

    Serial.printf("[WiFi] Found %d networks\n", n);
}

void WiFiManager::handleReconnect()
{
    if (is_ap_mode)
    {
        if (isConnected())
        {
            stopAPMode();
            return;
        }

        if (millis() - ap_start_time > ap_timeout)
        {
            Serial.println("[WiFi] AP timeout");
            delay(1000);
            ESP.restart();
        }
    }
    else
    {
        if (!isConnected())
        {
            unsigned long current_time = millis();

            if (current_time - last_reconnect_time >= reconnect_interval)
            {
                last_reconnect_time = current_time;
                WiFi.reconnect();
            }
        }
        else
        {
            is_sta_connected = true;
        }
    }
}

void WiFiManager::handleWebServer()
{
    if (is_ap_mode && web_server != nullptr)
    {
        dns_server.processNextRequest();
        web_server->handleClient();
    }
}

void WiFiManager::disconnect()
{
    WiFi.disconnect(true);
    is_sta_connected = false;
}

String WiFiManager::getAPSSID()
{
    return String(AP_SSID);
}

void WiFiManager::clearConfig()
{
    preferences.clear();
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
}

WiFiManager::~WiFiManager()
{
    if (web_server != nullptr)
    {
        delete web_server;
        web_server = nullptr;
    }
    preferences.end();
}
