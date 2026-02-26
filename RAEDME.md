# Pulmote-ESP

ESP32 智慧家居紅外線控制韌體

---

## 📌 專案簡介

Pulmote-ESP 是一個基於 ESP32 的智慧家居控制韌體，整合以下功能：

- 📡 紅外線 (IR) 訊號接收與發送
- 🌐 WiFi 無線網路連線
- 📬 MQTT 物聯網通訊
- 🏠 傳統家電智慧化控制

本專案可將傳統紅外線遙控家電（冷氣、電風扇、電視等）轉換為可透過手機或
MQTT 系統控制的智慧設備。

---

## 🧠 系統架構

手機 App / Web 控制頁面 │ │ WiFi ▼ ESP32 控制核心 │ ├── IR 發射 →
控制家電 ├── IR 接收 → 學習遙控器 └── MQTT → 雲端 / 本地 Broker

---

## 🔧 硬體需求

- ESP32 DevKit V3.0 (ESP32-WROOM-32)
- 紅外線接收模組（例如 VS1838B）
- 紅外線 LED + 電晶體驅動電路
- WiFi 網路環境
- MQTT Broker（例如 Mosquitto 或 Home Assistant）

---

## ⚙️ 開發環境

- VSCode
- PlatformIO
- Framework: Arduino
- Board: esp32dev

### 需要的函式庫

- IRremoteESP8266
- PubSubClient

---

## 🚀 主要功能

### 🔹 紅外線學習模式

- 接收遙控器訊號
- 過濾重複與雜訊訊號
- 支援 NEC / Sony / RC5 / RC6 等協定

### 🔹 紅外線發送

- 重送已學習訊號
- 支援多種協定
- 支援 Raw 訊號發送

### 🔹 WiFi 自動連線

- 開機自動連線
- 斷線自動重連

### 🔹 MQTT 物聯網整合

- 訂閱控制主題
- 發送設備狀態
- 可整合 Home Assistant

---

## 📡 MQTT 主題設計範例

home/livingroom/ac/set home/livingroom/ac/state home/livingroom/fan/set
home/livingroom/fan/state

---

## 🔌 腳位設定

功能 GPIO

---

IR 接收 15
IR 發射 4
狀態 LED 2

---

## 🛠 編譯與燒錄

pio run pio run --target upload pio device monitor

---

## 🔮 未來擴充方向

- OTA 無線更新
- Web 設定頁面
- SPIFFS 儲存紅外線資料
- 多房間設備管理
- Flutter 手機 App 整合

---

## 👨‍💻 專案作者

ESP32 智慧家居系統實作專案

---

## 📜 授權條款

MIT License
