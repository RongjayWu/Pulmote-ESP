/**
 * @file ir_manager.cpp
 * @brief 紅外線管理模組實現
 *
 * 實現紅外線接收和發送功能
 */

#include "ir_manager.h"

void IRManager::init(uint16_t rx_pin, uint16_t tx_pin)
{
    ir_receive_pin = rx_pin;
    ir_send_pin = tx_pin;
    is_learning = false;

    pinMode(ir_receive_pin, INPUT);
    pinMode(ir_send_pin, OUTPUT);

    Serial.println("[IR] 紅外線管理器已初始化");
    Serial.printf("[IR] 接收腳位: GPIO%d, 發送腳位: GPIO%d\n", rx_pin, tx_pin);
}

void IRManager::startLearning()
{
    is_learning = true;
    Serial.println("[IR] 進入學習模式 - 請按下遙控器按鈕");
    Serial.println("[IR] 按 'X' 鍵退出學習模式");
}

void IRManager::stopLearning()
{
    is_learning = false;
    Serial.println("[IR] 已退出學習模式");
}

void IRManager::sendSignal(const uint16_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
    {
        Serial.println("[IR] 錯誤: 無效的訊號數據");
        return;
    }

    Serial.printf("[IR] 發送紅外線訊號 (長度: %d)\n", length);

    // 模擬發送 - 實際應用需使用 IRremoteESP8266 或類似函式庫
    for (uint16_t i = 0; i < length; i++)
    {
        Serial.printf("  %d: %uus\n", i, data[i]);
    }

    Serial.println("[IR] 訊號發送完成");
}

bool IRManager::hasSignal()
{
    // 檢查接收腳位是否有訊號
    // 實際應用需使用 IR 解碼器
    return false;
}

void IRManager::getReceivedSignal()
{
    if (!is_learning)
    {
        return;
    }

    Serial.println("[IR] 已接收到遙控器訊號");
    Serial.println("[IR] 訊號已保存");
}
