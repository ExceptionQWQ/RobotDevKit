/*
 * @author BusyBox
 * @date 2024/5/1
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/motor/general/encoder_motor.h"

EncoderMotor::EncoderMotor(Mode mode)
{
    this->mode = mode;
    speed_pid = std::make_shared<PID>();
    pos_pid = std::make_shared<PID>(speed_pid);
    pos_pid->set_pid_type(PID::PIDType::Pos);
}

EncoderMotor::~EncoderMotor()
{

}


/*
 * @brief 每隔1ms调用一次
 */
void EncoderMotor::tick()
{
    switch (mode) {
        case Mode::SPEED:
            speed_mode_tick();
            break;
        case Mode::SPEED_POS:
            speed_pos_mode_tick();
            break;
    }
}

/*
 * @brief 设置电机是否反转
 */
void EncoderMotor::set_reverse(bool reverse)
{
    this->reverse = reverse;
}

/*
 * @brief 设置目标转速
 */
void EncoderMotor::set_target_rpm(double rpm)
{
    double range = rpm < 0 ? -rpm : rpm;
    if (pos_pid) pos_pid->set_output_range(-range, range);
    target_rpm = rpm;
}

/*
 * @brief 获取转子转速
 */
double EncoderMotor::get_rpm()
{
    return rotor_rpm;
}

/*
 * @brief 设置目标位置
 */
void EncoderMotor::set_target_pos(double target_pos)
{
    this->target_pos = target_pos;
}

/*
 * @brief 获取目标位置
 */
double EncoderMotor::get_pos()
{
    return rotor_pos;
}

/*
 * @brief 设置速度PID
 * @param kp p比例系数
 * @param ki i比例系数
 * @param kd d比例系数
 */
void EncoderMotor::set_speed_pid(double kp, double ki, double kd)
{
    if (speed_pid) {
        speed_pid->set_kp(kp);
        speed_pid->set_ki(ki);
        speed_pid->set_kd(kd);
    }
}

/*
 * @brief 设置位置PID
 * @param kp p比例系数
 * @param ki i比例系数
 * @param kd d比例系数
 */
void EncoderMotor::set_pos_pid(double kp, double ki, double kd)
{
    if (pos_pid) {
        pos_pid->set_kp(kp);
        pos_pid->set_ki(ki);
        pos_pid->set_kd(kd);
    }
}

/*
 * @brief 设置位置PID积分误差范围
 * @param min
 * @param max
 */
void EncoderMotor::set_pos_pid_total_error_range(double min, double max)
{
    if (pos_pid) {
        pos_pid->set_total_error_range(min, max);
    }
}

/*
 * @brief 设置速度PID输出PWM范围
 * @param min 最低值
 * @param max 最高值
 */
void EncoderMotor::set_speed_pid_output_range(double min, double max)
{
    if (speed_pid) {
        speed_pid->set_output_range(min, max);
    }
}

/*
 * @brief 设置位置PID输出速度范围
 * @param min 最低值
 * @param max 最高值
 */
void EncoderMotor::set_pos_pid_output_range(double min, double max)
{
    if (pos_pid) {
        pos_pid->set_output_range(min, max);
    }
}

/*
 * @brief 设置电机PWM输出回调函数
 */
void EncoderMotor::set_motor_pwm_output_callback(MotorPWMOutputCallback motor_pwm_output_callback)
{
    this->motor_pwm_output_callback = motor_pwm_output_callback;
}

/*
 * @brief 增加编码器脉冲数
 */
void EncoderMotor::add_pulse(int32_t pulse)
{
    if (reverse) pulse = -pulse;
    rotor_rpm = pulse;
    rotor_pos += pulse;
}

void EncoderMotor::speed_mode_tick()
{
    speed_pid->set_input(rotor_rpm);
    speed_pid->set_target(target_rpm);
    speed_pid->tick();

    if (reverse) {
        if (motor_pwm_output_callback) motor_pwm_output_callback(-speed_pid->get_output());
    } else {
        if (motor_pwm_output_callback) motor_pwm_output_callback(speed_pid->get_output());
    }
}

void EncoderMotor::speed_pos_mode_tick()
{
    speed_pid->set_input(rotor_rpm);
    speed_pid->set_target(pos_pid->get_output());
    pos_pid->set_input(rotor_pos);
    pos_pid->set_target(target_pos);
    pos_pid->tick();

    if (reverse) {
        if (motor_pwm_output_callback) motor_pwm_output_callback(-speed_pid->get_output());
    } else {
        if (motor_pwm_output_callback) motor_pwm_output_callback(speed_pid->get_output());
    }
}