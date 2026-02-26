/**
 * @file device_manager.cpp
 * @brief 家電設備管理模組實現
 *
 * 實現設備管理和控制功能
 */

#include "device_manager.h"

void DeviceManager::init()
{
    device_count = 0;
    Serial.println("[Device] 設備管理器已初始化");
}

bool DeviceManager::addDevice(uint8_t id, DeviceType type, const char *name)
{
    if (device_count >= MAX_DEVICES)
    {
        Serial.printf("[Device] 錯誤: 已達到最大設備數量 (%d)\n", MAX_DEVICES);
        return false;
    }

    devices[device_count].id = id;
    devices[device_count].type = type;
    strncpy(devices[device_count].name, name, sizeof(devices[device_count].name) - 1);
    devices[device_count].is_active = false;

    const char *type_name;
    switch (type)
    {
    case DEVICE_TV:
        type_name = "電視";
        break;
    case DEVICE_AC:
        type_name = "冷氣";
        break;
    case DEVICE_FAN:
        type_name = "電風扇";
        break;
    case DEVICE_LIGHT:
        type_name = "燈光";
        break;
    default:
        type_name = "未知";
        break;
    }

    Serial.printf("[Device] 添加設備: ID=%d, 類型=%s, 名稱=%s\n",
                  id, type_name, name);

    device_count++;
    return true;
}

void DeviceManager::sendCommand(uint8_t device_id, DeviceCommand command)
{
    // 查找設備
    int device_idx = -1;
    for (int i = 0; i < device_count; i++)
    {
        if (devices[i].id == device_id)
        {
            device_idx = i;
            break;
        }
    }

    if (device_idx == -1)
    {
        Serial.printf("[Device] 錯誤: 找不到設備 ID=%d\n", device_id);
        return;
    }

    Device &device = devices[device_idx];

    const char *command_name;
    switch (command)
    {
    case COMMAND_POWER_ON:
        command_name = "開機";
        break;
    case COMMAND_POWER_OFF:
        command_name = "關機";
        break;
    case COMMAND_POWER_TOGGLE:
        command_name = "切換電源";
        break;
    case COMMAND_VOLUME_UP:
        command_name = "音量增加";
        break;
    case COMMAND_VOLUME_DOWN:
        command_name = "音量減少";
        break;
    case COMMAND_CHANNEL_UP:
        command_name = "頻道增加";
        break;
    case COMMAND_CHANNEL_DOWN:
        command_name = "頻道減少";
        break;
    case COMMAND_TEMP_UP:
        command_name = "溫度升高";
        break;
    case COMMAND_TEMP_DOWN:
        command_name = "溫度降低";
        break;
    case COMMAND_MODE_COOL:
        command_name = "冷卻模式";
        break;
    case COMMAND_MODE_HEAT:
        command_name = "加熱模式";
        break;
    case COMMAND_FAN_SPEED_UP:
        command_name = "風速增加";
        break;
    case COMMAND_FAN_SPEED_DOWN:
        command_name = "風速降低";
        break;
    default:
        command_name = "未知";
        break;
    }

    Serial.printf("[Device] 發送指令: %s -> %s (%s)\n",
                  device.name, command_name,
                  command == COMMAND_POWER_ON ? "接通" : "");
}

bool DeviceManager::getDeviceStatus(uint8_t device_id)
{
    for (int i = 0; i < device_count; i++)
    {
        if (devices[i].id == device_id)
        {
            return devices[i].is_active;
        }
    }
    return false;
}

void DeviceManager::executeScenario(const char *scenario)
{
    Serial.printf("[Device] 執行場景: %s\n", scenario);

    if (strcmp(scenario, "movie") == 0)
    {
        // 看電影場景: 關閉燈光, 調整冷氣溫度
        Serial.println("[Device] 場景: 看電影 - 關閉燈光，調整冷氣溫度");
        sendCommand(3, COMMAND_POWER_OFF); // 關燈
        sendCommand(2, COMMAND_TEMP_DOWN); // 降低冷氣溫度
    }
    else if (strcmp(scenario, "sleep") == 0)
    {
        // 睡眠場景: 關閉所有設備
        Serial.println("[Device] 場景: 睡眠 - 關閉所有設備");
        sendCommand(1, COMMAND_POWER_OFF); // 關電視
        sendCommand(2, COMMAND_POWER_OFF); // 關冷氣
        sendCommand(3, COMMAND_POWER_OFF); // 關風扇
    }
    else if (strcmp(scenario, "away") == 0)
    {
        // 外出場景: 關閉所有家電
        Serial.println("[Device] 場景: 外出 - 關閉所有家電");
        for (int i = 0; i < device_count; i++)
        {
            sendCommand(devices[i].id, COMMAND_POWER_OFF);
        }
    }
    else
    {
        Serial.printf("[Device] 警告: 未知場景 '%s'\n", scenario);
    }
}

void DeviceManager::listDevices()
{
    Serial.println("\n========== 設備列表 ==========");

    if (device_count == 0)
    {
        Serial.println("沒有已添加的設備");
        Serial.println("=============================\n");
        return;
    }

    for (int i = 0; i < device_count; i++)
    {
        const char *type_name;
        switch (devices[i].type)
        {
        case DEVICE_TV:
            type_name = "電視";
            break;
        case DEVICE_AC:
            type_name = "冷氣";
            break;
        case DEVICE_FAN:
            type_name = "電風扇";
            break;
        case DEVICE_LIGHT:
            type_name = "燈光";
            break;
        default:
            type_name = "未知";
            break;
        }

        Serial.printf("[%d] %s (%s) - %s\n",
                      devices[i].id,
                      devices[i].name,
                      type_name,
                      devices[i].is_active ? "開啟" : "關閉");
    }

    Serial.println("=============================\n");
}

bool DeviceManager::removeDevice(uint8_t device_id)
{
    for (int i = 0; i < device_count; i++)
    {
        if (devices[i].id == device_id)
        {
            // 移除設備 - 向後移動其他設備
            for (int j = i; j < device_count - 1; j++)
            {
                devices[j] = devices[j + 1];
            }
            device_count--;
            Serial.printf("[Device] 設備 ID=%d 已移除\n", device_id);
            return true;
        }
    }

    Serial.printf("[Device] 錯誤: 找不到設備 ID=%d\n", device_id);
    return false;
}
