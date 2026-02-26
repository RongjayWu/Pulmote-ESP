/**
 * @file wifi_manager.cpp
 * @brief WiFi 管理模組實現
 *
 * 實現 WiFi 連接、狀態管理、AP 模式和 Web 配置功能
 */

#include "wifi_manager.h"

// ============== Web 伺服器 HTML 界面 ==============
const char WIFI_CONFIG_HTML[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Pulmote-ESP WiFi 配置</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 15px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.3);
            padding: 40px;
            max-width: 500px;
            width: 100%;
        }
        h1 {
            color: #333;
            margin-bottom: 10px;
            text-align: center;
            font-size: 28px;
        }
        .subtitle {
            color: #666;
            text-align: center;
            margin-bottom: 30px;
            font-size: 14px;
        }
        .status-box {
            background: #f5f5f5;
            border-left: 4px solid #667eea;
            padding: 15px;
            margin-bottom: 25px;
            border-radius: 5px;
        }
        .status-item {
            display: flex;
            justify-content: space-between;
            padding: 8px 0;
            font-size: 14px;
        }
        .status-label { color: #666; font-weight: 500; }
        .status-value { color: #333; font-weight: bold; }
        .status-value.connected { color: #4CAF50; }
        .status-value.disconnected { color: #f44336; }
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 8px;
            color: #333;
            font-weight: 500;
            font-size: 14px;
        }
        select, input[type="password"], input[type="text"] {
            width: 100%;
            padding: 12px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            font-size: 14px;
            transition: border-color 0.3s;
        }
        select:focus, input:focus {
            outline: none;
            border-color: #667eea;
        }
        .network-list {
            max-height: 200px;
            overflow-y: auto;
            margin-bottom: 15px;
        }
        .network-item {
            padding: 12px;
            background: #f9f9f9;
            border: 1px solid #e0e0e0;
            border-radius: 5px;
            margin-bottom: 8px;
            cursor: pointer;
            transition: all 0.2s;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .network-item:hover {
            background: #f0f0f0;
            border-color: #667eea;
        }
        .network-name { font-weight: 500; color: #333; }
        .network-signal { font-size: 12px; color: #999; }
        button {
            width: 100%;
            padding: 12px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            transition: transform 0.2s, box-shadow 0.2s;
            margin-top: 10px;
        }
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 20px rgba(102, 126, 234, 0.4);
        }
        button:active {
            transform: translateY(0);
        }
        .loading {
            display: none;
            text-align: center;
            margin: 20px 0;
        }
        .spinner {
            border: 4px solid #f3f3f3;
            border-top: 4px solid #667eea;
            border-radius: 50%;
            width: 40px;
            height: 40px;
            animation: spin 1s linear infinite;
            margin: 0 auto;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        .message {
            padding: 12px;
            border-radius: 5px;
            margin-bottom: 15px;
            display: none;
            font-size: 14px;
        }
        .message.success {
            background: #c8e6c9;
            color: #2e7d32;
            border: 1px solid #81c784;
        }
        .message.error {
            background: #ffcdd2;
            color: #c62828;
            border: 1px solid #e57373;
        }
        .message.info {
            background: #bbdefb;
            color: #1565c0;
            border: 1px solid #64b5f6;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Pulmote-ESP</h1>
        <p class="subtitle">WiFi 配置助手</p>

        <div class="status-box">
            <div class="status-item">
                <span class="status-label">STA 連線:</span>
                <span id="sta-status" class="status-value disconnected">未連接</span>
            </div>
            <div class="status-item">
                <span class="status-label">AP 模式:</span>
                <span id="ap-status" class="status-value connected">啟用中</span>
            </div>
            <div class="status-item">
                <span class="status-label">信號強度:</span>
                <span id="signal-strength" class="status-value">-- dBm</span>
            </div>
        </div>

        <div id="message" class="message"></div>

        <div class="form-group">
            <label>可用的 WiFi 網路</label>
            <button type="button" onclick="scanNetworks()">🔄 掃描網路</button>
            <div class="network-list" id="network-list">
                <p style="text-align: center; color: #999; padding: 20px;">
                    點擊掃描按鈕查找可用網路...
                </p>
            </div>
        </div>

        <form onsubmit="saveConfig(event)">
            <div class="form-group">
                <label for="ssid">網路名稱 (SSID)</label>
                <input type="text" id="ssid" placeholder="輸入或選擇 WiFi 名稱" required>
            </div>

            <div class="form-group">
                <label for="password">密碼</label>
                <input type="password" id="password" placeholder="輸入 WiFi 密碼">
            </div>

            <div class="loading" id="loading">
                <div class="spinner"></div>
                <p style="margin-top: 10px; color: #666;">正在連接...</p>
            </div>

            <button type="submit">✓ 保存並連接</button>
        </form>
    </div>

    <script>
        async function updateStatus()
{
    try
    {
        const response = await fetch('/api/status');
        const data = await response.json();

        document.getElementById('sta-status').textContent = data.sta_connected ? '已連接' : '未連接';
        document.getElementById('sta-status').className = data.sta_connected ? 'status-value connected' : 'status-value disconnected';
        document.getElementById('signal-strength').textContent = data.rssi + ' dBm';
    }
    catch (e)
    {
        console.log('更新狀態失敗:', e);
    }
}

async function scanNetworks()
{
    const listDiv = document.getElementById('network-list');
    listDiv.innerHTML = '<div class="spinner"></div><p style="text-align: center; margin-top: 10px;">掃描中...</p>';

    try
    {
        const response = await fetch('/api/scan');
        const networks = await response.json();

        if (networks.length == = 0)
        {
            listDiv.innerHTML = '<p style="text-align: center; color: #999; padding: 20px;">未找到可用網路</p>';
            return;
        }

        listDiv.innerHTML = '';
        networks.forEach(net = > {
            const item = document.createElement('div');
            item.className = 'network-item';
            item.innerHTML = ` <div>
                             <div class = "network-name"> ${net.ssid} < / div >
                             <div class = "network-signal"> 信號 : ${net.rssi} dBm</ div>
                                                                   </ div>
                    `;
            item.onclick = () =>
            {
                document.getElementById('ssid').value = net.ssid;
                document.getElementById('password').focus();
            };
            listDiv.appendChild(item);
        });
    }
    catch (e)
    {
        listDiv.innerHTML = '<p style="text-align: center; color: #f44336; padding: 20px;">掃描失敗</p>';
        console.log('掃描失敗:', e);
    }
}

async function saveConfig(event)
{
    event.preventDefault();

    const ssid = document.getElementById('ssid').value;
    const password = document.getElementById('password').value;

    if (!ssid)
    {
        showMessage('請輸入網路名稱', 'error');
        return;
    }

    document.getElementById('loading').style.display = 'block';

    try
    {
        const response = await fetch('/api/save', {
            method : 'POST',
            headers : {'Content-Type' : 'application/json'},
            body : JSON.stringify({ssid, password})
        });

        const result = await response.json();

        if (result.success)
        {
            showMessage('配置已保存，設備將在 5 秒後重啟...', 'success');
        }
        else
        {
            showMessage('保存失敗: ' + result.message, 'error');
            document.getElementById('loading').style.display = 'none';
        }
    }
    catch (e)
    {
        showMessage('請求失敗: ' + e.message, 'error');
        document.getElementById('loading').style.display = 'none';
    }
}

function showMessage(text, type)
{
    const msgDiv = document.getElementById('message');
    msgDiv.textContent = text;
    msgDiv.className = 'message ' + type;
    msgDiv.style.display = 'block';

    if (type != = 'error')
    {
        setTimeout(() = > {
            msgDiv.style.display = 'none';
        },
                   5000);
    }
}

// 頁面加載時更新狀態
updateStatus();
setInterval(updateStatus, 3000);
    </script>
</body>
</html>
)";

// ============== WiFiManager 实现 ==============

WiFiManager::WiFiManager() : web_server(nullptr), is_sta_connected(false), is_ap_mode(false), ap_start_time(0)
    {
        memset(ssid, 0, sizeof(ssid));
        memset(password, 0, sizeof(password));
    }

    void WiFiManager::init()
    {
        Serial.println("[WiFi] 初始化 WiFi 管理器...");

        // 初始化 NVS（密鑰-值存儲）
        if (!preferences.begin("wifi_config", false))
        {
            Serial.println("[WiFi] 警告: NVS 初始化失敗");
        }

        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.persistent(false);

        is_sta_connected = false;
        is_ap_mode = false;
        last_reconnect_time = 0;

        // 嘗試加載保存的 WiFi 配置
        if (loadConfigFromNVS())
        {
            Serial.printf("[WiFi] 已加載保存的配置: SSID=%s\n", ssid);
            // 嘗試連接到保存的網路
            if (connect(ssid, password))
            {
                is_sta_connected = true;
                Serial.println("[WiFi] WiFi 連接成功");
            }
            else
            {
                Serial.println("[WiFi] WiFi 連接失敗，進入 AP 模式");
                startAPMode();
            }
        }
        else
        {
            Serial.println("[WiFi] 未找到保存的 WiFi 配置，進入 AP 模式");
            startAPMode();
        }

        Serial.println("[WiFi] WiFi 管理器初始化完成");
    }

    bool WiFiManager::loadConfigFromNVS()
    {
        // 檢查是否存在保存的 SSID
        if (!preferences.isKey("ssid"))
        {
            return false;
        }

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
        Serial.printf("[WiFi] 配置已保存到 NVS: %s\n", _ssid);
    }

    void WiFiManager::initWebServer()
    {
        if (web_server == nullptr)
        {
            web_server = new WebServer(80);

            // 設置路由處理程序
            web_server->on("/", HTTP_GET, [this]()
                           { handleRoot(); });
            web_server->on("/api/scan", HTTP_GET, [this]()
                           { handleScanNetworks(); });
            web_server->on("/api/save", HTTP_POST, [this]()
                           { handleSaveConfig(); });
            web_server->on("/api/status", HTTP_GET, [this]()
                           { handleStatus(); });

            web_server->begin();
            Serial.println("[WiFi AP] Web 服務器已啟動 (http://192.168.4.1)");
        }
    }

    void WiFiManager::handleRoot()
    {
        web_server->send(200, "text/html; charset=utf-8", WIFI_CONFIG_HTML);
    }

    void WiFiManager::handleScanNetworks()
    {
        Serial.println("[WiFi AP] 正在掃描 WiFi 網路...");

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
            json += ",\"secure\":";
            json += (WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "true" : "false");
            json += "}";
        }

        json += "]";
        web_server->send(200, "application/json", json);
    }

    void WiFiManager::handleSaveConfig()
    {
        if (!web_server->hasArg("plain"))
        {
            web_server->send(400, "application/json", "{\"success\":false,\"message\":\"無效的請求\"}");
            return;
        }

        String body = web_server->arg("plain");

        // 簡單的 JSON 解析
        int ssid_start = body.indexOf("\"ssid\":\"") + 8;
        int ssid_end = body.indexOf("\"", ssid_start);
        String new_ssid = body.substring(ssid_start, ssid_end);

        int pass_start = body.indexOf("\"password\":\"") + 12;
        int pass_end = body.indexOf("\"", pass_start);
        String new_password = body.substring(pass_start, pass_end);

        if (new_ssid.length() == 0)
        {
            web_server->send(400, "application/json", "{\"success\":false,\"message\":\"SSID 不能為空\"}");
            return;
        }

        // 保存配置
        saveConfigToNVS(new_ssid.c_str(), new_password.c_str());
        strncpy(ssid, new_ssid.c_str(), sizeof(ssid) - 1);
        strncpy(password, new_password.c_str(), sizeof(password) - 1);

        web_server->send(200, "application/json", "{\"success\":true,\"message\":\"配置已保存，設備將在 5 秒後重啟\"}");

        // 5 秒後重啟設備
        delay(5000);
        ESP.restart();
    }

    void WiFiManager::handleStatus()
    {
        String json = "{";
        json += "\"sta_connected\":" + String(is_sta_connected ? "true" : "false") + ",";
        json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
        json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
        json += "\"ap_mode\":" + String(is_ap_mode ? "true" : "false");
        json += "}";

        web_server->send(200, "application/json", json);
    }

    void WiFiManager::startAPMode()
    {
        Serial.println("[WiFi AP] 啟動 AP 模式...");

        is_ap_mode = true;
        ap_start_time = millis();

        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
        bool ap_ok = WiFi.softAP(AP_SSID, AP_PASSWORD);

        if (ap_ok)
        {
            Serial.printf("[WiFi AP] AP 模式已啟動\n");
            Serial.printf("[WiFi AP] SSID: %s\n", AP_SSID);
            Serial.printf("[WiFi AP] Password: %s\n", AP_PASSWORD);
            Serial.printf("[WiFi AP] IP: %s\n", WiFi.softAPIP().toString().c_str());
            Serial.println("[WiFi AP] 請使用手機連接此 WiFi，然後訪問 http://192.168.4.1");

            initWebServer();
        }
        else
        {
            Serial.println("[WiFi AP] AP 模式啟動失敗");
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

            Serial.println("[WiFi] 已關閉 AP 模式");
        }
    }

    bool WiFiManager::connect(const char *_ssid, const char *_password)
    {
        Serial.printf("[WiFi] 正在連接到 '%s'...\n", _ssid);

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

            Serial.println("\n✓ WiFi 連接成功");
            Serial.printf("[WiFi] 本地 IP: %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("[WiFi] 信號強度: %d dBm\n", WiFi.RSSI());

            return true;
        }
        else
        {
            is_sta_connected = false;
            Serial.println("\n✗ WiFi 連接失敗");
            Serial.printf("[WiFi] WiFi 狀態: %d\n", WiFi.status());

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
        Serial.println("[WiFi] 掃描可用 WiFi 網路...");
        int n = WiFi.scanNetworks();

        if (n == 0)
        {
            Serial.println("[WiFi] 未找到任何網路");
            return;
        }

        Serial.printf("[WiFi] 找到 %d 個網路:\n", n);

        for (int i = 0; i < n; ++i)
        {
            Serial.printf("  %d. SSID: %s | 信號: %d dBm | 加密: %d\n",
                          i + 1,
                          WiFi.SSID(i).c_str(),
                          WiFi.RSSI(i),
                          WiFi.encryptionType(i));
        }
    }

    void WiFiManager::handleReconnect()
    {
        // 檢查 AP 模式超時
        if (is_ap_mode)
        {
            // 如果 STA 已連接，關閉 AP 模式
            if (isConnected())
            {
                stopAPMode();
                return;
            }

            // 如果 AP 模式超時（10 分鐘），則重啟設備
            if (millis() - ap_start_time > ap_timeout)
            {
                Serial.println("[WiFi AP] AP 模式超時，設備將重啟");
                delay(1000);
                ESP.restart();
            }
        }
        else
        {
            // STA 模式下的重連邏輯
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
                is_sta_connected = true;
            }
        }
    }

    void WiFiManager::handleWebServer()
    {
        if (is_ap_mode && web_server != nullptr)
        {
            web_server->handleClient();
        }
    }

    void WiFiManager::disconnect()
    {
        WiFi.disconnect(true);
        is_sta_connected = false;
        Serial.println("[WiFi] WiFi 已斷開");
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
        Serial.println("[WiFi] WiFi 配置已清除");
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
