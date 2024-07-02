/*
 * @author BusyBox
 * @date 2024/7/2
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/servo/servo.h"

Servo::Servo(uint8_t id)
{
    this->id = id;
}

/*
 * @brief 舵机扭矩开关
 * @param value true 打开扭矩输出 false 关闭扭矩输出
 */
void Servo::torque_switch(bool value)
{

}

/*
 * @brief 直接读取舵机角度传感器的位置值
 * @details 对于使用12bit磁传感器的舵机，返回的值的范围为0-4095
 * @return position
 */
int16_t Servo::read_position()
{
    return position;
}

/*
 * @brief 读取上一次写入的位置值
 * @details 对于使用12bit磁传感器的舵机，返回的值的范围为0-4095
 * @return position
 */
int16_t Servo::read_position_cache()
{
    return position;
}

/*
 * @brief 写入位置值
 * @param position
 */
void Servo::write_position(int16_t position)
{
    this->position = position;
}

/*
 * @brief 写入位置和速度
 * @param position
 * @param speed 步/s
 */
void Servo::write_position_speed(int16_t position, uint16_t speed)
{
    this->position = position;
}