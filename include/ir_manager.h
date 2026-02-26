#ifndef IR_MANAGER_H
#define IR_MANAGER_H

#include <Arduino.h>

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
private:
    uint16_t ir_receive_pin;
    uint16_t ir_send_pin;
    bool is_learning;

public:
    /**
     * @brief 初始化紅外線管理器
     * @param rx_pin 接收腳位
     * @param tx_pin 發送腳位
     */
    void init(uint16_t rx_pin, uint16_t tx_pin);

    /**
     * @brief 進入學習模式 - 接收遙控器訊號
     * @return 學習到的訊號數據
     */
    void startLearning();

    /**
     * @brief 停止學習模式
     */
    void stopLearning();

    /**
     * @brief 發送紅外線訊號
     * @param data 訊號數據
     * @param length 訊號長度
     */
    void sendSignal(const uint16_t *data, uint16_t length);

    /**
     * @brief 檢查是否有接收到訊號
     * @return 是否有新訊號
     */
    bool hasSignal();

    /**
     * @brief 獲取接收到的訊號
     */
    void getReceivedSignal();
};

#endif // IR_MANAGER_H
