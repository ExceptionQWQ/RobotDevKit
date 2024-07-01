/*
 * @author BusyBox
 * @date 2024/5/2
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <memory>
#include <cmath>
#include "rdk/core/motor/motor.h"

/*
     * @brief 两轮差速运动模型
     *
     *  |-----------------------|
     *  |                       |
     *  |                       |
     *  |                       |
     *  | motor1         motor2 |
     *  |                       |
     *  |                       |
     *  |                       |
     *  |-----------------------|
     *
     *  小车前方为x轴，左边为y轴
     *  电机转向应该满足当所有电机正转时，机器人沿x正半轴行驶
     *
     */
class DifferentialMotion
{
public:
    DifferentialMotion(std::shared_ptr<Motor> motor1, std::shared_ptr<Motor> motor2);
    ~DifferentialMotion();

    void tick();
    void set_wheel_radius(double wheel_radius);
    void clear();
    void add_x_speed(double speed);
    void add_z_speed(double speed);
    void commit();
private:

    std::shared_ptr<Motor> motor1;
    std::shared_ptr<Motor> motor2;

    double motor1_rpm = 0;
    double motor2_rpm = 0;

    double pi = 3.1415926;

    double wheel_radius = 10;
};