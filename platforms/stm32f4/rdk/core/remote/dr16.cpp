/*
 * @author BusyBox
 * @date 2024/6/30
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/remote/dr16.h"

DR16::DR16(std::shared_ptr<SerialPort> serialPort)
{
    this->serialPort = serialPort;
    alive_tick = HAL_GetTick();
}

DR16::~DR16()
{

}

/*
 * @brief 启动接收遥控器函数
 * @attention 请单独放在一个线程中，该函数为阻塞函数，调用后不会返回
 */
void DR16::start()
{
    while (true) {
        std::size_t recv_bytes = serialPort->read(dbus_data.recv_buff, 64, 5); //遥控器每隔7ms发送18字节数据
        if (recv_bytes == 18) { //利用超时机制获取完整的18字节数据包
            alive_tick = HAL_GetTick();
            remote_data.channel_0 = dbus_data.channel_0;
            remote_data.channel_1 = dbus_data.channel_1;
            remote_data.channel_2 = dbus_data.channel_2;
            remote_data.channel_3 = dbus_data.channel_3;
            remote_data.s1 = dbus_data.s1;
            remote_data.s2 = dbus_data.s2;
            remote_data.mouse_x = dbus_data.mouse_x;
            remote_data.mouse_y = dbus_data.mouse_y;
            remote_data.mouse_z = dbus_data.mouse_z;
            remote_data.mouse_left = dbus_data.mouse_left;
            remote_data.mouse_right = dbus_data.mouse_right;
        }
    }
}

/*
 * @brief 获取通道0的数据
 * @details 最小值 364 中间值 1024 最大值 1684
 */
uint16_t DR16::get_channel_0()
{
    return remote_data.channel_0;
}

/*
 * @brief 获取通道1的数据
 * @details 最小值 364 中间值 1024 最大值 1684
 */
uint16_t DR16::get_channel_1()
{
    return remote_data.channel_1;
}

/*
 * @brief 获取通道2的数据
 * @details 最小值 364 中间值 1024 最大值 1684
 */
uint16_t DR16::get_channel_2()
{
    return remote_data.channel_2;
}

/*
 * @brief 获取通道3的数据
 * @details 最小值 364 中间值 1024 最大值 1684
 */
uint16_t DR16::get_channel_3()
{
    return remote_data.channel_3;
}

/*
 * @brief 获取s1的数据
 * @details 最小值 1 最大值 3
 */
uint8_t DR16::get_s1()
{
    return remote_data.s1;
}

/*
 * @brief 获取s2的数据
 * @details 最小值 1 最大值 3
 */
uint8_t DR16::get_s2()
{
    return remote_data.s2;
}

/*
 * @brief 通信是否活跃
 * @retrun true 通信正常 false 通信断开
 */
bool DR16::alive()
{
    if (HAL_GetTick() - alive_tick < timeout) return true;
    else return false;
}