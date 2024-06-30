/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

class Motor
{
public:
    Motor() = default;
    ~Motor() = default;

    virtual void tick() = 0;
    virtual void set_reverse(bool reverse) = 0;
    virtual void set_target_rpm(double rpm) = 0;
    virtual double get_rpm() = 0;

private:

};