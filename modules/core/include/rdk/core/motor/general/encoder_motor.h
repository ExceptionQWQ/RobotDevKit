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

    /*
     * @brief 每隔1ms调用一次
     */
    void tick() override;
    /*
     * @brief 设置电机是否反转
     */
    void set_reverse(bool reverse) override;
    /*
     * @brief 设置目标转速
     */
    void set_target_rpm(double rpm) override;
    /*
     * @brief 获取转子转速
     */
    double get_rpm();
    /*
     * @brief 设置目标位置
     */
    virtual void set_target_pos(double target_pos);
    /*
     * @brief 获取目标位置
     */
    virtual double get_pos();
    /*
     * @brief 设置速度PID
     * @param kp p比例系数
     * @param ki i比例系数
     * @param kd d比例系数
     */
    void set_speed_pid(double kp, double ki, double kd);
    /*
     * @brief 设置位置PID
     * @param kp p比例系数
     * @param ki i比例系数
     * @param kd d比例系数
     */
    void set_pos_pid(double kp, double ki, double kd);
    /*
     * @brief 设置位置PID积分误差范围
     * @param min
     * @param max
     */
    void set_pos_pid_total_error_range(double min, double max);
    /*
     * @brief 设置速度PID输出PWM范围
     * @param min 最低值
     * @param max 最高值
     */
    void set_speed_pid_output_range(double min, double max);
    /*
     * @brief 设置位置PID输出速度范围
     * @param min 最低值
     * @param max 最高值
     */
    void set_pos_pid_output_range(double min, double max);
    /*
     * @brief 设置电机PWM输出回调函数
     */
    void set_motor_pwm_output_callback(MotorPWMOutputCallback motor_pwm_output_callback);
    /*
     * @brief 每隔一定时间对增量式编码器采样，然后调用add_pulse传入电机速度增量
     */
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

