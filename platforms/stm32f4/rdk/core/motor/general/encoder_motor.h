/*
 * @author BusyBox
 * @date 2024/5/1
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include "rdk/core/control/pid.h"
#include "rdk/core/motor/motor.h"

class EncoderMotor : public Motor
{
public:

    using MotorPWMOutputCallback = std::function<void(double pwm)>;

    enum class Mode : uint8_t
    {
        SPEED = 0x01, //速度模式
        SPEED_POS = 0x02, //速度位置模式，默认启动后的位置为0度
    };

    EncoderMotor(Mode mode);
    ~EncoderMotor();

    void tick() override;
    void set_reverse(bool reverse) override;
    void set_target_rpm(double rpm) override;
    double get_rpm();
    virtual void set_target_pos(double target_pos);
    virtual double get_pos();
    void set_speed_pid(double kp, double ki, double kd);
    void set_pos_pid(double kp, double ki, double kd);
    void set_pos_pid_total_error_range(double min, double max);
    void set_speed_pid_output_range(double min, double max);
    void set_pos_pid_output_range(double min, double max);
    void set_motor_pwm_output_callback(MotorPWMOutputCallback motor_pwm_output_callback);
    void add_pulse(int32_t pulse);

private:
    Mode mode;
    bool reverse = false;
    MotorPWMOutputCallback motor_pwm_output_callback;

    double target_rpm = 0;
    double target_pos = 0;
    double rotor_rpm = 0;
    double rotor_pos = 0;

    std::shared_ptr<PID> speed_pid;
    std::shared_ptr<PID> pos_pid;

    void speed_mode_tick();
    void speed_pos_mode_tick();

};

