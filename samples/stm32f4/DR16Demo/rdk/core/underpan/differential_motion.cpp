/*
 * @author BusyBox
 * @date 2024/5/2
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/underpan/differential_motion.h"

DifferentialMotion::DifferentialMotion(std::shared_ptr<Motor> motor1, std::shared_ptr<Motor> motor2)
{
    this->motor1 = motor1;
    this->motor2 = motor2;
}

DifferentialMotion::~DifferentialMotion()
{

}

/*
 * @brief 每隔1ms调用一次tick，会自动调用2个motor的tick函数
 */
void DifferentialMotion::tick()
{
    if (motor1) motor1->tick();
    if (motor2) motor2->tick();
}

/*
 * @brief 设置轮子半径
 * @param wheel_radius 轮子半径(mm)
 */
void DifferentialMotion::set_wheel_radius(double wheel_radius)
{
    this->wheel_radius = wheel_radius;
}

/*
 * @brief 清除缓存区的速度
 */
void DifferentialMotion::clear()
{
    motor1_rpm = 0;
    motor2_rpm = 0;
}

/*
 * @brief 添加x轴速度
 * @param speed (mm/s)
 */
void DifferentialMotion::add_x_speed(double speed)
{
    motor1_rpm += speed * 30 / pi / wheel_radius;
    motor2_rpm += speed * 30 / pi / wheel_radius;
}

/*
 * @brief 添加z轴速度
 * @param speed
 */
void DifferentialMotion::add_z_speed(double speed)
{
    motor1_rpm -= speed * 30 / pi / wheel_radius;
    motor2_rpm += speed * 30 / pi / wheel_radius;
}

/*
 * @brief 将缓冲区的速度写入电机
 */
void DifferentialMotion::commit()
{
    if (motor1) motor1->set_target_rpm(motor1_rpm);
    if (motor2) motor2->set_target_rpm(motor2_rpm);
}
