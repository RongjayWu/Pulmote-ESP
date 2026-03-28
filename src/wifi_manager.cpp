// WiFiManager 模組 Source
#include "wifi_manager.h"
#include "wifi_ap_html.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

WiFiManager::WiFiManager() // 建構子初始化
{
    dev_status_pin = 0;       // 預設狀態指示燈腳位
    webServer = nullptr;      // 初始化 Web Server 指標為空
    lastAttemptSsid = "";     // 初始化上次嘗試連接的 SSID 為空
    lastAttemptPassword = ""; // 初始化上次嘗試連接的密碼為空
    lastBlinkMillis = 0;
    ledState = false;
    blinkIntervalMs = 200; // 0.2 秒閃爍
}

void WiFiManager::init(uint16_t status_pin) // WiFi 初始化流程
{
    dev_status_pin = status_pin;
    pinMode(dev_status_pin, OUTPUT);   // 設定狀態指示燈腳位為輸出
    digitalWrite(dev_status_pin, LOW); // 預設狀態指示燈為關閉
    // 初始化 Preferences（用於儲存/讀取 WiFi 與 AP 設定）
    preferences.begin("wifi_config", false); // 讀寫模式
    // 讀取並印出偏好設定內容供除錯
    String storedSsid = preferences.getString("ssid", "");
    String storedPwd = preferences.getString("password", "");
    Serial.printf("Preferences read: ssid='%s' password_len=%u\n", storedSsid.c_str(), (unsigned)storedPwd.length());
    WiFi.mode(WIFI_MODE_STA); // 設定 WiFi 模式為 Station
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WiFiManager: already connected to WiFi");
    }
    else if (storedSsid != "" && storedPwd != "") // 如果有儲存的SSID以及密碼，嘗試連接
    {
        Serial.println("WiFiManager: attempting to connect to WiFi with stored credentials");
        WiFi.begin(storedSsid.c_str(), storedPwd.c_str());
    }
    else
    {
        Serial.println("WiFiManager: not connected to WiFi, starting AP mode");
        startAPMode(); // 如果未連接，啟動 AP 模式
    }
    preferences.end(); // 關閉 Preferences，釋放資源
}

void WiFiManager::handleConnect()
{
    // If we have a cached user attempt, try that first
    if (this->lastAttemptSsid.length() > 0)
    {
        Serial.printf("WiFiManager: handleConnect - trying cached ssid='%s'\n", this->lastAttemptSsid.c_str());
        WiFi.mode(WIFI_MODE_APSTA);
        WiFi.begin(this->lastAttemptSsid.c_str(), this->lastAttemptPassword.c_str());
        if (!webServer)
            startWebServer();
        return;
    }

    // Otherwise read stored credentials from Preferences and try
}
// Debug endpoints removed per request

void WiFiManager::startWebServer() // 處理 Web Server 的 Client 請求
{
    if (webServer)
        return;
    webServer = new WebServer(80);

    webServer->on("/", [this]()
                  {
        if (webServer) webServer->send(200, "text/html", WIFI_AP_HTML); });

    webServer->onNotFound([this]()
                          {
        if (webServer) webServer->send(404, "text/plain", "Not Found"); });

    // Scan networks (blocking). Returns JSON array of {ssid,rssi}.
    webServer->on("/scan", HTTP_GET, [this]()
                  {
        int n = WiFi.scanNetworks();
        String out = "[]";
        if (n > 0) {
            out = "[";
            for (int i = 0; i < n; ++i) {
                if (i) out += ',';
                out += '{';
                out += String("\"ssid\":\"") + WiFi.SSID(i) + String("\"");
                out += String(",\"rssi\":") + String(WiFi.RSSI(i));
                out += '}';
            }
            out += ']';
        }
        if (webServer) webServer->send(200, "application/json", out); });

    // Connect (POST form: ssid, pass)
    webServer->on("/connect", HTTP_POST, [this]()
                  {
        String ssid = webServer->hasArg("ssid") ? webServer->arg("ssid") : String();
        String pass = webServer->hasArg("pass") ? webServer->arg("pass") : String();
        if (ssid.length() == 0) {
            if (webServer) webServer->send(400, "text/plain", "ssid required");
            return;
        }
        Serial.printf("HTTP /connect received ssid='%s' pass_len=%u\n", ssid.c_str(), (unsigned)pass.length());
        // cache last attempt
        this->lastAttemptSsid = ssid;
        this->lastAttemptPassword = pass;
        // persist credentials so device can auto reconnect later
        preferences.begin("wifi_config", false);
        preferences.putString("ssid", ssid);
        preferences.putString("password", pass);
        Serial.printf("Preferences write: ssid='%s' password_len=%u\n", ssid.c_str(), (unsigned)pass.length());
        preferences.end();
        // start connecting
        WiFi.mode(WIFI_MODE_APSTA);
        WiFi.begin(ssid.c_str(), pass.c_str());
        if (webServer) webServer->send(200, "text/plain", "connecting"); });

    // No status/debug endpoints (removed per request)

    webServer->begin();
}

void WiFiManager::stopWebServer()
{
    if (!webServer)
        return;
    webServer->stop();
    delete webServer;
    webServer = nullptr;
}
void WiFiManager::startAPMode() // 啟動 AP 模式
{
    if (isAPActive())
    {
        return; // 已經為 AP 模式，無需重啟
    }
    Serial.println("WiFiManager: starting AP mode");
    // 使用 AP+STA 模式，避免影響 Station 功能
    WiFi.mode(WIFI_MODE_APSTA);
    // 臨時開啟 Preferences、讀取 AP SSID / 密碼，讀取完立即 end()
    preferences.begin("wifi_config", false);
    String apSsid = preferences.getString("ap_ssid", "Pulmote-ESP");
    String apPass = preferences.getString("ap_pass", "Pulmote-ESP");
    Serial.printf("Preferences read (AP): ap_ssid='%s' ap_pass_len=%u\n", apSsid.c_str(), (unsigned)apPass.length());
    preferences.end();
    bool ok = false;
    if (apPass.length() >= 8)
    {
        Serial.println("WiFiManager: starting secured softAP");
        ok = WiFi.softAP(apSsid.c_str(), apPass.c_str());
    }
    else
    {
        Serial.println("WiFiManager: starting open softAP (note: password must be at least 8 characters for a secured AP)");
        ok = WiFi.softAP(apSsid.c_str());
    }
    if (!ok)
    {
        Serial.println("WiFiManager: softAP start failed");
    }
    // 啟動 Web Server 供設定使用
    startWebServer();
}

void WiFiManager::stopAPMode()
{
    if (!isAPActive())
    {
        return; // 非 AP 模式，無需處理
    }
    Serial.println("WiFiManager: stopping AP mode");
    // 停止 Web Server
    stopWebServer();
    // 停用 softAP（true 表示等待關閉）
    WiFi.softAPdisconnect(true);
    // 將模式設回 STA（若需要保留 STA 能力）
    WiFi.mode(WIFI_MODE_STA);
}

WiFiManager::~WiFiManager()
{
    if (webServer)
    {
        webServer->stop();
        delete webServer;
        webServer = nullptr;
    }
    // Ensure Preferences is closed; safe to call even if not open
    preferences.end();
}

bool WiFiManager::isAPActive()
{
    // 對 ESP32 Arduino core：WiFi.getMode() 回傳 wifi_mode_t
    // 當模式為 WIFI_MODE_AP 或 WIFI_MODE_APSTA 時表示 AP 已啟用
    wifi_mode_t mode = WiFi.getMode();
    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA)
    {
        return true;
    }
    return false;
}

void WiFiManager::statusPinControl() // 控制狀態指示燈
{
    switch (WiFi.status())
    {
    case WL_DISCONNECTED:
        /* 未連線：以非阻塞方式每 0.2 秒切換 LED指示燈 */
        if ((unsigned long)(millis() - lastBlinkMillis) >= blinkIntervalMs)
        {
            lastBlinkMillis = millis();
            ledState = !ledState;
            digitalWrite(dev_status_pin, ledState ? HIGH : LOW);
        }
        break;
    case WL_CONNECTED:
        /* 已連線： 常亮並重置閃爍狀態*/
        digitalWrite(dev_status_pin, HIGH);
        ledState = false;
        lastBlinkMillis = millis();
        break;
    case WL_CONNECT_FAILED:
        /* 連線失敗：以非阻塞方式每 0.2 秒切換 LED指示燈*/
        if ((unsigned long)(millis() - lastBlinkMillis) >= blinkIntervalMs)
        {
            lastBlinkMillis = millis();
            ledState = !ledState;
            digitalWrite(dev_status_pin, ledState ? HIGH : LOW);
        }
        break;
    default:
        break;
    }
}

void WiFiManager::loop() // 主循環處理
{
    static int prevWifiStatus = -1;
    int currWifiStatus = WiFi.status();

    // Handle status transitions for persist-on-success and clear-on-fail
    if (currWifiStatus != prevWifiStatus)
    {
        if (currWifiStatus == WL_CONNECTED)
        {
            if (this->lastAttemptSsid.length() > 0 && WiFi.SSID() == this->lastAttemptSsid)
            {
                // Persist credentials on successful connection
                preferences.begin("wifi_config", false);
                preferences.putString("ssid", this->lastAttemptSsid);
                preferences.putString("password", this->lastAttemptPassword);
                preferences.end();
                Serial.printf("WiFiManager: persisted credentials for ssid='%s'\n", this->lastAttemptSsid.c_str());
                // clear cached attempt
                this->lastAttemptSsid = "";
                this->lastAttemptPassword = "";
            }
        }
        else if (currWifiStatus == WL_CONNECT_FAILED)
        {
            // On immediate connect failure, clear cached attempt to avoid persisting wrong password
            if (this->lastAttemptSsid.length() > 0)
            {
                Serial.println("WiFiManager: connect failed for cached attempt, clearing cached credentials");
                this->lastAttemptSsid = "";
                this->lastAttemptPassword = "";
            }
        }
        prevWifiStatus = currWifiStatus;
    }

    switch (currWifiStatus)
    {
    case WL_DISCONNECTED:
        /*如果WiFi未連接且AP Mode未開啟，則開啟AP Mode*/
        if (!WiFi.isConnected() && !isAPActive())
        {
            startAPMode();
        }
        break;
    case WL_CONNECTED:
        /*關閉AP Mode並且關閉web server*/
        if (WiFi.isConnected() && isAPActive())
        {
            stopAPMode();
        }
        break;
    case WL_CONNECT_FAILED:
        /*保持AP Mode開啟並且開啟web server*/
        if (!WiFi.isConnected() && !isAPActive())
        {
            startAPMode();
        }
        break;
    default:
        break;
    }
    statusPinControl(); // 控制狀態指示燈
    // 處理 WebServer client（若啟動）
    if (webServer)
        webServer->handleClient();
}
