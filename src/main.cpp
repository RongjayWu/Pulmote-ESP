/**
 * @file main.cpp
 * @brief Pulmote-ESP 主程序入口
 *
 * 功能:
 * - 初始化所有模組（WiFi、MQTT、IR、設備管理）
 * - 設置中斷和事件處理
 * - 主循環管理
 */

#include <Arduino.h>
#include "ir_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "device_manager.h"
#include "ble_manager.h"

// ============== 全域物件宣告 ==============
IRManager ir_manager;
WiFiManager wifi_manager;
MQTTManager mqtt_manager;
DeviceManager device_manager;
BLEManager ble_manager;

// ============== 引腳配置 ==============
const uint8_t IR_RX_PIN = 15; // GPIO15 - IR 接收
const uint8_t IR_TX_PIN = 4;  // GPIO4 - IR 發送
const uint8_t LED_PIN = 2;    // GPIO2 - 狀態 LED

// ============== MQTT 配置 ==============
const char *MQTT_BROKER = "192.168.1.100";
const uint16_t MQTT_PORT = 1883;
const char *MQTT_CLIENT_ID = "pulmote-esp32";

// ============== 時序控制 ==============
unsigned long last_status_publish = 0;
const unsigned long STATUS_PUBLISH_INTERVAL = 30000; // 30秒發佈一次狀態

// ============== MQTT 訊息回調函數 ==============
void onMQTTMessage(const char *topic, const char *payload)
{
  Serial.printf("[MQTT] Topic: %s, Payload: %s\n", topic, payload);

  // 解析 MQTT 指令進行控制
  if (strstr(topic, "command") != NULL)
  {
    // 例如: pulmote/device/1/command -> 控制設備 1
    if (strstr(payload, "power_toggle") != NULL)
    {
      device_manager.sendCommand(1, COMMAND_POWER_TOGGLE);
    }
  }
}

// ============== LED 指示燈控制 ==============
void updateLEDStatus()
{
  if (wifi_manager.isConnected())
  {
    digitalWrite(LED_PIN, HIGH); // WiFi 已連接，LED 常亮
  }
  else if (wifi_manager.isAPMode())
  {
    // AP 模式下，LED 閃爍（300ms 亮，100ms 暗）
    unsigned long current = millis();
    if ((current % 400) < 300)
    {
      digitalWrite(LED_PIN, HIGH);
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
    }
  }
  else
  {
    digitalWrite(LED_PIN, LOW); // 未連接，LED 滅
  }
}

// ============== 初始化函數 ==============
void setup()
{
  // 清除 WiFi 設定並重啟（僅用於重置，完成後請移除）
  // wifi_manager.clearConfig();
  // wifi_manager.disconnect();
  // wifi_manager.startAPMode();
  // ESP.restart();
  // 初始化序列埠（用於調試）
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n\n╔════════════════════════════════════╗");
  Serial.println("║     Pulmote-ESP 系統啟動中...      ║");
  Serial.println("╚════════════════════════════════════╝\n");

  // 初始化 LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 1. 初始化 IR 管理器
  Serial.println("[1/4] 初始化紅外線管理器...");
  ir_manager.init(IR_RX_PIN, IR_TX_PIN);
  Serial.println("✓ IR 管理器已初始化\n");

  // 2. 初始化 WiFi 管理器
  Serial.println("[2/4] 初始化 WiFi 管理器...");
  wifi_manager.init();
  Serial.println("✓ WiFi 管理器已初始化\n");

  // 2.5 初始化 BLE 管理器
  Serial.println("[2.5/4] 初始化 BLE 管理器...");
  ble_manager.init();
  Serial.println("✓ BLE 管理器已初始化\n");

  // 3. 初始化 MQTT （延遲，等待 WiFi 連接）
  Serial.println("[3/4] 初始化 MQTT...");
  mqtt_manager.init();
  mqtt_manager.setCallback(onMQTTMessage);
  Serial.println("✓ MQTT 已初始化\n");

  // 4. 初始化設備管理器
  Serial.println("[4/4] 初始化設備管理器...");
  device_manager.init();
  // 添加示例設備
  device_manager.addDevice(1, DEVICE_TV, "客廳電視");
  device_manager.addDevice(2, DEVICE_AC, "臥室冷氣");
  device_manager.addDevice(3, DEVICE_FAN, "客廳風扇");
  Serial.println("✓ 設備管理器已初始化\n");

  Serial.println("╔════════════════════════════════════╗");
  Serial.println("║     所有模組初始化完成！           ║");
  Serial.println("╚════════════════════════════════════╝\n");

  // 顯示 WiFi 狀態
  if (wifi_manager.isAPMode())
  {
    Serial.println("⚠️  當前在 AP 配置模式");
    Serial.println("   SSID: Pulmote-ESP");
    Serial.println("   Password: 123456789");
    Serial.println("   IP: http://192.168.4.1");
    Serial.println("   請使用手機連接此 WiFi 並進行配置\n");
  }
  else if (wifi_manager.isConnected())
  {
    Serial.println("✓ WiFi 已連接");
    Serial.printf("  IP: %s\n\n", wifi_manager.getLocalIP().c_str());

    // 連接 MQTT
    if (mqtt_manager.connect(MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID))
    {
      Serial.println("✓ MQTT 連接成功");
      mqtt_manager.subscribe("pulmote/device/+/command");
      mqtt_manager.subscribe("pulmote/scene/command");
      Serial.println();
    }
  }
}

// ============== 主循環函數 ==============
void loop()
{
  // 更新 LED 狀態
  updateLEDStatus();

  // 處理 WiFi 重連和 AP 超時
  wifi_manager.handleReconnect();

  // 處理 Web 服務器（AP 模式配置）
  wifi_manager.handleWebServer();

  // 處理 MQTT 訊息
  if (wifi_manager.isConnected() && mqtt_manager.isConnected())
  {
    mqtt_manager.loop();
  }

  // 呼叫 BLE 管理器的 loop 函數
  ble_manager.loop();
  // 檢查是否有 IR 訊號
  if (ir_manager.hasSignal())
  {
    ir_manager.getReceivedSignal();
  }

  // 定期發佈設備狀態到 MQTT
  unsigned long current_time = millis();
  if (current_time - last_status_publish >= STATUS_PUBLISH_INTERVAL)
  {
    last_status_publish = current_time;

    if (wifi_manager.isConnected() && mqtt_manager.isConnected())
    {
      // 發佈狀態訊息
      mqtt_manager.publish("pulmote/status/online", "true", true);
      mqtt_manager.publish("pulmote/status/wifi",
                           wifi_manager.isConnected() ? "connected" : "disconnected");
      mqtt_manager.publish("pulmote/status/mqtt",
                           mqtt_manager.isConnected() ? "connected" : "disconnected");
    }
  }

  delay(10); // 防止 WDT 重啟
}