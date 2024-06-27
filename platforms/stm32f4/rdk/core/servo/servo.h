/*
 * @author BusyBox
 * @date 2024/5/26
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>

class Servo
{
public:
    Servo(uint8_t id);

    /*
     * @brief 舵机扭矩开关
     * @param value true 打开扭矩输出 false 关闭扭矩输出
     */
    virtual void torque_switch(bool value);

    /*
     * @brief 直接读取舵机角度传感器的位置值
     * @details 对于使用12bit磁传感器的舵机，返回的值的范围为0-4095
     * @return position
     */
    virtual int16_t read_position();

    /*
     * @brief 读取上一次写入的位置值
     * @details 对于使用12bit磁传感器的舵机，返回的值的范围为0-4095
     * @return position
     */
    virtual int16_t read_position_cache();

    /*
     * @brief 写入位置值
     * @param position
     */
    virtual void write_position(int16_t position);

    /*
     * @brief 写入位置和速度
     * @param position
     * @param speed 步/s
     */
    virtual void write_position_speed(int16_t position, uint16_t speed);


protected:
    uint8_t id = 0;
    int16_t position = 0;

private:

};