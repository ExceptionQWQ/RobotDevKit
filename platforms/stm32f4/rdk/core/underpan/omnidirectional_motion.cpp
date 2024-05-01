/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/underpan/omnidirectional_motion.h"

OmnidirectionalMotion::OmnidirectionalMotion(std::shared_ptr<Motor> motor1, std::shared_ptr<Motor> motor2, std::shared_ptr<Motor> motor3, std::shared_ptr<Motor> motor4)
{
    this->motor1 = motor1;
    this->motor2 = motor2;
    this->motor3 = motor3;
    this->motor4 = motor4;
    clear();
    commit();
}

OmnidirectionalMotion::~OmnidirectionalMotion()
{

}

/*
 * @brief 每隔1ms调用一次tick，会自动调用4个motor的tick函数
 */
void OmnidirectionalMotion::tick()
{
    if (motor1) motor1->tick();
    if (motor2) motor2->tick();
    if (motor3) motor3->tick();
    if (motor4) motor4->tick();
}

/*
 * @brief 设置轮子半径
 * @param wheel_radius 轮子半径(mm)
 */
void OmnidirectionalMotion::set_wheel_radius(double wheel_radius)
{
    this->wheel_radius = wheel_radius;
}

/*
 * @brief 清除缓存区的速度
 */
void OmnidirectionalMotion::clear()
{
    motor1_rpm = 0;
    motor2_rpm = 0;
    motor3_rpm = 0;
    motor4_rpm = 0;
}

/*
 * @brief 添加x轴速度
 * @param speed (mm/s)
 */
void OmnidirectionalMotion::add_x_speed(double speed)
{
    motor1_rpm += -speed / sqrt2 * 30 / pi / wheel_radius;
    motor2_rpm += speed / sqrt2 * 30 / pi / wheel_radius;
    motor3_rpm += speed / sqrt2 * 30 / pi / wheel_radius;
    motor4_rpm += -speed / sqrt2 * 30 / pi / wheel_radius;
}

/*
 * @brief 添加y轴速度
 * @param speed (mm/s)
 */
void OmnidirectionalMotion::add_y_speed(double speed)
{
    motor1_rpm += speed / sqrt2 * 30 / pi / wheel_radius;
    motor2_rpm += speed / sqrt2 * 30 / pi / wheel_radius;
    motor3_rpm += -speed / sqrt2 * 30 / pi / wheel_radius;
    motor4_rpm += -speed / sqrt2 * 30 / pi / wheel_radius;
}

/*
 * @brief 添加z轴速度
 * @param speed
 */
void OmnidirectionalMotion::add_z_speed(double speed)
{
    motor1_rpm += speed / sqrt2 * 30 / pi / wheel_radius;
    motor2_rpm += speed / sqrt2 * 30 / pi / wheel_radius;
    motor3_rpm += speed / sqrt2 * 30 / pi / wheel_radius;
    motor4_rpm += speed / sqrt2 * 30 / pi / wheel_radius;
}

/*
 * @brief 将缓冲区的速度写入电机
 */
void OmnidirectionalMotion::commit()
{
    if (motor1) motor1->set_target_rpm(motor1_rpm);
    if (motor2) motor2->set_target_rpm(motor2_rpm);
    if (motor3) motor3->set_target_rpm(motor3_rpm);
    if (motor4) motor4->set_target_rpm(motor4_rpm);
}


