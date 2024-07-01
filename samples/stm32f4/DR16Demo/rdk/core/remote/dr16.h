/*
 * @author BusyBox
 * @date 2024/6/30
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include "rdk/core/transfer/serial_port.h"

/*
 * DR16接收器
 * CubeMX中配置串口时需注意 Baud Rate为100000，Word Length为9bits，Parity为Even，Stop Bits为1
 *
 */

class DR16
{
public:
    DR16(std::shared_ptr<SerialPort> serialPort);
    ~DR16();

    /*
     * @brief 启动接收遥控器函数
     * @attention 请单独放在一个线程中，该函数为阻塞函数，调用后不会返回
     */
    void start();

    /*
     * @brief 获取通道0的数据
     * @details 最小值 364 中间值 1024 最大值 1684
     */
    uint16_t get_channel_0();

    /*
     * @brief 获取通道1的数据
     * @details 最小值 364 中间值 1024 最大值 1684
     */
    uint16_t get_channel_1();

    /*
     * @brief 获取通道2的数据
     * @details 最小值 364 中间值 1024 最大值 1684
     */
    uint16_t get_channel_2();

    /*
     * @brief 获取通道3的数据
     * @details 最小值 364 中间值 1024 最大值 1684
     */
    uint16_t get_channel_3();

    /*
     * @brief 获取s1的数据
     * @details 最小值 1 最大值 3
     */
    uint8_t get_s1();

    /*
     * @brief 获取s2的数据
     * @details 最小值 1 最大值 3
     */
    uint8_t get_s2();

    /*
     * @brief 通信是否活跃
     * @retrun true 通信正常 false 通信断开
     */
    bool alive();


private:
    std::shared_ptr<SerialPort> serialPort;
    uint32_t timeout = 100; //通信超时时间
    uint32_t alive_tick = 0;

#pragma pack(push, 1)
    union DBUS_Data_Def
    {
        struct {
            uint16_t channel_0:11;
            uint16_t channel_1:11;
            uint16_t channel_2:11;
            uint16_t channel_3:11;
            uint8_t s1:2;
            uint8_t s2:2;
            uint16_t mouse_x:16;
            uint16_t mouse_y:16;
            uint16_t mouse_z:16;
            uint8_t mouse_left:8;
            uint8_t mouse_right:8;
            uint16_t key:16;
        };
        uint8_t data[18];
        uint8_t recv_buff[64];
    }dbus_data;
#pragma pack(pop)

    struct RemoteData
    {
        uint16_t channel_0 = 1024;
        uint16_t channel_1 = 1024;
        uint16_t channel_2 = 1024;
        uint16_t channel_3 = 1024;
        uint8_t s1 = 1;
        uint8_t s2 = 1;
        uint16_t mouse_x = 0;
        uint16_t mouse_y = 0;
        uint16_t mouse_z = 0;
        uint8_t mouse_left = 0;
        uint8_t mouse_right = 0;
    }remote_data;

};

