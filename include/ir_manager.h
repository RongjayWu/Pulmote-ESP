#ifndef IR_MANAGER_H
#define IR_MANAGER_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>

/**
 * @file ir_manager.h
 * @brief 紅外線管理模組 - 處理 IR 接收/發送
 *
 * 功能:
 * - 接收紅外線訊號（學習模式）
 * - 發送紅外線訊號控制家電
 * - 儲存和播放學習到的遙控器指令
 */

class IRManager
{
public:
    IRManager();
    void init(uint16_t rx_pin, uint16_t tx_pin, uint16_t status_pin);
    void startLearning();
    void stopLearning();
    void sendSignal(const uint16_t *data, uint16_t length);
    bool hasSignal();
    void getReceivedSignal();
    void loop();
    ~IRManager();

private:
    uint16_t ir_receive_pin;
    uint16_t ir_send_pin;
    uint16_t dev_status_pin;
    bool is_learning;
};

#endif // IR_MANAGER_H
