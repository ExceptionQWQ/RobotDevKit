/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <memory>
#include <cmath>
#include "rdk/core/motor/motor.h"



/*
     * @brief 4个全向轮
     *
     *  |-----------------------|
     *  | motor1         motor2 |
     *  |                       |
     *  |                       |
     *  |                       |
     *  |                       |
     *  |                       |
     *  |                       |
     *  |                       |
     *  |                       |
     *  | motor4         motor3 |
     *  |-----------------------|
     *
     *  小车前方为x轴，左边为y轴
     *  电机转向应该满足当所有电机正转时，机器人逆时针旋转
     *
     */
class OmnidirectionalMotion
{
public:
    OmnidirectionalMotion(std::shared_ptr<Motor> motor1, std::shared_ptr<Motor> motor2, std::shared_ptr<Motor> motor3, std::shared_ptr<Motor> motor4);
    ~OmnidirectionalMotion();

    void tick();
    void set_wheel_radius(double wheel_radius);
    void clear();
    void add_x_speed(double speed);
    void add_y_speed(double speed);
    void add_z_speed(double speed);
    void commit();

private:
    std::shared_ptr<Motor> motor1;
    std::shared_ptr<Motor> motor2;
    std::shared_ptr<Motor> motor3;
    std::shared_ptr<Motor> motor4;

    double motor1_rpm = 0;
    double motor2_rpm = 0;
    double motor3_rpm = 0;
    double motor4_rpm = 0;

    double sqrt2 = std::sqrt(2);
    double pi = 3.1415926;

    double wheel_radius = 76;

};