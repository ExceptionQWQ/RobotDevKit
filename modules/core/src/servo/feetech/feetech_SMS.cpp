/*
 * @author BusyBox
 * @date 2024/5/26
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/servo/feetech/feetech_SMS.h"

FeetechSMS::FeetechSMS(std::shared_ptr<FeetechProtocol> fee, uint8_t id) : Servo(id)
{
    this->fee = fee;
}

FeetechSMS::~FeetechSMS()
{

}

/*
 * @brief 舵机扭矩开关
 * @param value true 打开扭矩输出 false 关闭扭矩输出
 */
void FeetechSMS::torque_switch(bool value)
{
    fee->torque_switch(id, value);
}

/*
 * @brief 直接读取舵机角度传感器的位置值
 * @details 对于使用12bit磁传感器的舵机，返回的值的范围为0-4095
 * @return position
 */
int16_t FeetechSMS::read_position()
{
    int16_t pos = 0;
    fee->read_position(id, &pos, false);
    return pos;
}

/*
 * @brief 读取上一次写入的位置值
 * @details 对于使用12bit磁传感器的舵机，返回的值的范围为0-4095
 * @return position
 */
int16_t FeetechSMS::read_position_cache()
{
    return Servo::read_position_cache();
}

/*
 * @brief 写入位置值
 * @param position
 */
void FeetechSMS::write_position(int16_t position)
{
    Servo::write_position(position);
    fee->write_position(id, position, false);
}

/*
 * @brief 写入位置和速度
 * @param position
 * @param speed 步/s
 */
void FeetechSMS::write_position_speed(int16_t position, uint16_t speed)
{
    Servo::write_position_speed(position, speed);
    fee->write_position_speed(id, position, speed, false);
}
