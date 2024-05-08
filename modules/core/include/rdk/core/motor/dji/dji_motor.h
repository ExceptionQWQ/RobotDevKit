/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 */

#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include "rdk/core/control/pid.h"
#include "rdk/core/motor/motor.h"
#include "rdk/core/motor/dji/c6xx_controller.h"

class DjiMotor : public Motor
{
public:

    enum class Mode : uint8_t
    {
        SPEED = 0x01, //速度模式
        SPEED_POS = 0x02, //速度位置模式，默认启动后的位置为0度
    };

    DjiMotor(std::shared_ptr<C6xxController> c6xx_controller, int id, Mode mode = Mode::SPEED);
    ~DjiMotor();

    void tick() override;
    virtual void set_max_output_current(double current);
    void set_target_rpm(double rpm) override;
    virtual void set_target_pos(double target_pos);
    double get_rpm() override;
    virtual double get_pos();
    void set_speed_pid(double kp, double ki, double kd);
    void set_pos_pid(double kp, double ki, double kd);
    void set_pos_pid_total_error_range(double min, double max);
    void set_reverse(bool reverse) override;

protected:
    double max_output_current = 0;
    void set_speed_pid_output_range(double min, double max);
    void set_pos_pid_output_range(double min, double max);


private:
    int id;
    std::shared_ptr<C6xxController> c6xx_controller;
    Mode mode;
    bool reverse = false;

    double target_rpm;
    double target_pos;
    double rotor_rpm;
    double rotor_pos;
    double last_rotor_pos = 0;
    double pos = 0;

    std::shared_ptr<PID> speed_pid;
    std::shared_ptr<PID> pos_pid;

    void speed_mode_tick();
    void speed_pos_mode_tick();
    double get_rotor_rpm();
    double get_rotor_pos();
    void calc_pos();

};