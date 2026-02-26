#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <Arduino.h>

/**
 * @file device_manager.h
 * @brief 家電設備管理模組 - 管理和控制家電裝置
 *
 * 功能:
 * - 定義支持的家電設備類型
 * - 發送裝置控制指令
 * - 儲存設備配置
 * - 執行預設場景（如「看電影」、「睡眠模式」）
 */

enum DeviceType
{
    DEVICE_TV,    // 電視
    DEVICE_AC,    // 冷氣
    DEVICE_FAN,   // 電風扇
    DEVICE_LIGHT, // 燈光
    DEVICE_UNKNOWN
};

enum DeviceCommand
{
    COMMAND_POWER_ON,
    COMMAND_POWER_OFF,
    COMMAND_POWER_TOGGLE,
    COMMAND_VOLUME_UP,
    COMMAND_VOLUME_DOWN,
    COMMAND_CHANNEL_UP,
    COMMAND_CHANNEL_DOWN,
    COMMAND_TEMP_UP,
    COMMAND_TEMP_DOWN,
    COMMAND_MODE_COOL,
    COMMAND_MODE_HEAT,
    COMMAND_FAN_SPEED_UP,
    COMMAND_FAN_SPEED_DOWN
};

struct Device
{
    uint8_t id;
    DeviceType type;
    char name[32];
    bool is_active;
};

class DeviceManager
{
private:
    static const uint8_t MAX_DEVICES = 10;
    Device devices[MAX_DEVICES];
    uint8_t device_count;

public:
    /**
     * @brief 初始化設備管理器
     */
    void init();

    /**
     * @brief 添加新設備
     * @param id 設備 ID
     * @param type 設備類型
     * @param name 設備名稱
     */
    bool addDevice(uint8_t id, DeviceType type, const char *name);

    /**
     * @brief 發送控制指令到設備
     * @param device_id 設備 ID
     * @param command 控制指令
     */
    void sendCommand(uint8_t device_id, DeviceCommand command);

    /**
     * @brief 獲取設備狀態
     * @param device_id 設備 ID
     * @return 設備是否活躍
     */
    bool getDeviceStatus(uint8_t device_id);

    /**
     * @brief 執行場景（多個指令的組合）
     * @param scenario 場景名稱 ("movie", "sleep", "away" 等)
     */
    void executeScenario(const char *scenario);

    /**
     * @brief 列出所有設備
     */
    void listDevices();

    /**
     * @brief 移除設備
     * @param device_id 設備 ID
     */
    bool removeDevice(uint8_t device_id);
};

#endif // DEVICE_MANAGER_H
