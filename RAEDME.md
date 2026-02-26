# Pulmote-ESP

ESP32 智慧家居紅外線控制韌體

---

## 📌 專案簡介

Pulmote-ESP 是一個基於 ESP32 的智慧家居控制韌體，整合以下功能：

- 📡 紅外線 (IR) 訊號接收與發送
- 🌐 WiFi 無線網路連線
- 📬 MQTT 物聯網通訊
- 🏠 傳統家電智慧化控制

本專案可將傳統紅外線遙控家電（冷氣、電風扇、電視等）轉換為可透過手機或 MQTT 系統控制的智慧設備。

---

## 🧠 系統架構

```
        手機 App / Web 控制頁面
                │
               WiFi
                │
                ▼
        ┌─────────────────┐
        │  ESP32 控制核心 │
        ├─────────────────┤
        │  IR 發射 → 控制家電
        │  IR 接收 → 學習遙控器
        │  MQTT → 雲端 / 本地 Broker
        └─────────────────┘
```

### 軟體模組設計

```
Pulmote-ESP/
│
├── src/
│   ├── main.cpp           # 核心入口，負責初始化
│   ├── ir_manager.cpp     # 處理紅外線接收/發送
│   ├── wifi_manager.cpp   # 處理 WiFi 連線
│   ├── mqtt_manager.cpp   # 處理 MQTT 通訊
│   └── device_manager.cpp # 管理家電裝置指令
│
├── include/
│   ├── ir_manager.h       # IR 管理器頭文件
│   ├── wifi_manager.h     # WiFi 管理器頭文件
│   ├── mqtt_manager.h     # MQTT 管理器頭文件
│   └── device_manager.h   # 設備管理器頭文件
│
└── platformio.ini         # 專案設定檔
```

---

## 🔧 硬體需求

- **微控制器**: ESP32 DevKit V3.0 (ESP32-WROOM-32)
- **紅外線接收**: VS1838B 紅外線接收模組
- **紅外線發射**:
  - 紅外線 LED (940nm)
  - NPN 電晶體 (2N2222 或同等)
  - 限流電阻 (220Ω)
- **網路**: WiFi 2.4GHz 網路環境
- **MQTT Broker**: Mosquitto、Home Assistant 或其他 MQTT 服務

### 接線圖範例

```
紅外線接收:    GPIO15 ── VS1838B (OUT) ── GND
              3.3V ── VS1838B (VCC)
              GND ── VS1838B (GND)

紅外線發射:    GPIO4 ── 2N2222 (Base) ── 220Ω 電阻 ── GND
              5V ── IR LED ── 2N2222 (Collector)
              2N2222 (Emitter) ── GND
```

---

## ⚙️ 開發環境

### 必需工具

- **編輯器**: VSCode
- **開發框架**: PlatformIO
- **語言**: C++ (Arduino Framework)
- **目標板**: esp32dev

### 必要的函式庫

```ini
lib_deps =
  crankyoldgit/IRremoteESP8266    # 紅外線控制
  knolleary/PubSubClient@^2.8.0   # MQTT 通訊
```

### 開發環境安裝

1. **安裝 PlatformIO**:

   ```bash
   # VS Code 中安裝 PlatformIO 擴充套件
   ```

2. **克隆或建立專案**:

   ```bash
   git clone <repository> Pulmote-ESP
   cd Pulmote-ESP
   ```

3. **編譯專案**:

   ```bash
   pio run
   ```

4. **編譯並燒錄**:

   ```bash
   pio run --target upload
   ```

5. **監控序列埠輸出**:
   ```bash
   pio device monitor --baud 115200
   ```

---

## 🚀 主要模組功能

### 1️⃣ IR Manager (`ir_manager.h` / `ir_manager.cpp`)

**功能概述**:

- 接收紅外線訊號（學習模式）
- 發送紅外線訊號控制家電
- 儲存和播放學習到的遙控器指令

**主要 API**:

```cpp
void init(uint16_t rx_pin, uint16_t tx_pin);      // 初始化
void startLearning();                               // 進入學習模式
void stopLearning();                                // 退出學習模式
void sendSignal(const uint16_t* data, uint16_t length); // 發送訊號
bool hasSignal();                                   // 檢查是否有訊號
void getReceivedSignal();                           // 獲取接收訊號
```

**使用範例**:

```cpp
ir_manager.init(15, 4);        // GPIO15 接收，GPIO4 發射
ir_manager.startLearning();     // 開始學習

// 發送訊號
uint16_t signal[] = {560, 560, 560, 1680, 560, 560, ...};
ir_manager.sendSignal(signal, sizeof(signal)/sizeof(signal[0]));
```

---

### 2️⃣ WiFi Manager (`wifi_manager.h` / `wifi_manager.cpp`)

**功能概述**:

- 連接到指定 WiFi 網路
- 監控 WiFi 連線狀態
- 自動重連機制
- 掃描可用網路

**主要 API**:

```cpp
void init();                                    // 初始化
bool connect(const char* ssid, const char* pwd); // 連接 WiFi
bool isConnected();                             // 檢查連線狀態
String getLocalIP();                            // 獲取本地 IP
void scanNetworks();                            // 掃描網路
void handleReconnect();                         // 處理重連邏輯
void disconnect();                              // 斷開連線
```

**使用範例**:

```cpp
wifi_manager.init();
if (wifi_manager.connect("MySSID", "MyPassword")) {
    Serial.println(wifi_manager.getLocalIP()); // 取得 IP
}

// 在主循環中定期檢查重連
wifi_manager.handleReconnect();
```

---

### 3️⃣ MQTT Manager (`mqtt_manager.h` / `mqtt_manager.cpp`)

**功能概述**:

- 連接到 MQTT Broker
- 訂閱和發送訊息
- 訊息回調處理
- 自動保持連線

**主要 API**:

```cpp
void init();                                    // 初始化
bool connect(const char* broker, uint16_t port, const char* client_id);
bool subscribe(const char* topic);              // 訂閱主題
void publish(const char* topic, const char* payload, bool retain = false);
bool isConnected();                             // 檢查連線狀態
void setCallback(mqtt_callback_t callback);     // 設置訊息回調
void loop();                                    // 訊息循環
void disconnect();                              // 斷開連線
```

**使用範例**:

```cpp
mqtt_manager.init();
mqtt_manager.connect("192.168.1.100", 1883, "pulmote-esp32");

// 設置訊息回調
mqtt_manager.setCallback([](const char* topic, const char* payload) {
    Serial.printf("Topic: %s, Payload: %s\n", topic, payload);
});

// 訂閱主題
mqtt_manager.subscribe("pulmote/device/+/command");

// 發送訊息
mqtt_manager.publish("pulmote/status/online", "true", true);
```

---

### 4️⃣ Device Manager (`device_manager.h` / `device_manager.cpp`)

**功能概述**:

- 定義支持的家電設備類型
- 發送裝置控制指令
- 儲存設備配置
- 執行預設場景

**支持的設備類型**:

- `DEVICE_TV` - 電視
- `DEVICE_AC` - 冷氣
- `DEVICE_FAN` - 電風扇
- `DEVICE_LIGHT` - 燈光

**支持的控制指令**:

- `COMMAND_POWER_ON` - 開機
- `COMMAND_POWER_OFF` - 關機
- `COMMAND_POWER_TOGGLE` - 切換電源
- `COMMAND_VOLUME_UP` / `COMMAND_VOLUME_DOWN` - 音量控制
- `COMMAND_TEMP_UP` / `COMMAND_TEMP_DOWN` - 溫度控制
- 等等...

**主要 API**:

```cpp
void init();                                    // 初始化
bool addDevice(uint8_t id, DeviceType type, const char* name);
void sendCommand(uint8_t device_id, DeviceCommand command);
bool getDeviceStatus(uint8_t device_id);       // 取得設備狀態
void executeScenario(const char* scenario);     // 執行場景
void listDevices();                             // 列出所有設備
bool removeDevice(uint8_t device_id);           // 移除設備
```

**預設場景**:

- `"movie"` - 看電影場景（關燈、調冷氣）
- `"sleep"` - 睡眠場景（關閉所有設備）
- `"away"` - 外出場景（關閉所有家電）

**使用範例**:

```cpp
device_manager.init();

// 添加設備
device_manager.addDevice(1, DEVICE_TV, "客廳電視");
device_manager.addDevice(2, DEVICE_AC, "臥室冷氣");

// 發送控制指令
device_manager.sendCommand(1, COMMAND_POWER_ON);

// 執行場景
device_manager.executeScenario("movie");

// 列出所有設備
device_manager.listDevices();
```

---

## 📡 MQTT 主題設計

### 主題結構

```
pulmote/device/{id}/command          - 設備控制命令
pulmote/device/{id}/state            - 設備狀態
pulmote/scene/command                - 場景執行命令
pulmote/status/online                - 設備在線狀態
pulmote/status/wifi                  - WiFi 連線狀態
pulmote/status/mqtt                  - MQTT 連線狀態
```

### 訊息格式範例

**控制電視開機**:

```
主題: pulmote/device/1/command
載荷: {"action": "power_on"}
```

**查詢設備狀態**:

```
主題: pulmote/device/1/state
載荷: {"status": "on", "device_name": "客廳電視"}
```

**執行看電影場景**:

```
主題: pulmote/scene/command
載荷: {"scene": "movie"}
```

---

## 🔌 腳位配置表

| 功能     | GPIO | 備註                   |
| -------- | ---- | ---------------------- |
| IR 接收  | 15   | INPUT，連接 VS1838B    |
| IR 發射  | 4    | OUTPUT，驅動 IR LED    |
| 狀態 LED | 2    | OUTPUT，連接狀態指示燈 |

---

## 🛠 編譯與燒錄

### 編譯

```bash
pio run
```

### 編譯並燒錄

```bash
pio run --target upload
```

### 監控序列埠輸出

```bash
pio run --target upload && pio device monitor --baud 115200
```

### 清除編譯結果

```bash
pio run --target clean
```

---

## 🔧 配置說明

### WiFi 配置

編輯 `src/main.cpp` 中的以下部分:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
```

### MQTT 配置

編輯 `src/main.cpp` 中的以下部分:

```cpp
const char* MQTT_BROKER = "192.168.1.100";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "pulmote-esp32";
```

### GPIO 配置

編輯 `src/main.cpp` 中的引腳定義:

```cpp
const uint8_t IR_RX_PIN = 15;    // 改為實際的接收腳位
const uint8_t IR_TX_PIN = 4;     // 改為實際的發射腳位
const uint8_t LED_PIN = 2;       // 改為實際的 LED 腳位
```

---

## 🔮 未來擴充方向

- ⬜ OTA 無線更新功能
- ⬜ Web 設定入口頁面
- ⬜ SPIFFS 檔案系統儲存紅外線資料
- ⬜ 多房間設備管理
- ⬜ Flutter/React 手機 App 整合
- ⬜ Home Assistant 完整整合
- ⬜ 本地語音控制（Microphone）
- ⬜ 室內溫濕度感測

---

## 🐛 除錯技巧

### 序列埠監控

```bash
pio device monitor --baud 115200
```

啟動後可在序列埠監控看到初始化日誌:

```
=== Pulmote-ESP 啟動中 ===

[1/4] 初始化紅外線管理器...
✓ IR 管理器已初始化
[2/4] 初始化 WiFi...
✓ WiFi 已初始化
...
```

### 常見問題

**Q: WiFi 無法連接**

- 檢查 SSID 和密碼
- 確保 ESP32 的天線在有信號覆蓋的位置
- 檢查路由器是否允許 2.4GHz 連接

**Q: MQTT 無法連接**

- 確認 MQTT Broker 地址和端口正確
- 檢查防火牆設定
- 確認網路連通性: `ping <broker-ip>`

**Q: IR 訊號無法接收**

- 檢查紅外線接收模組連線
- 確認 GPIO15 無其他衝突
- 電源是否穩定

---

## 👨‍💻 專案作者

ESP32 智慧家居系統實作專案

---

## 📜 授權條款

MIT License

---

## 📞 聯絡與支援

如有問題或建議，歡迎提出 Issue 或 Pull Request。
