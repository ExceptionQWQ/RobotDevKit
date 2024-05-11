/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/motor/dji/dji_motor.h"

DjiMotor::DjiMotor(std::shared_ptr<C6xxController> c6xx_controller, int id, Mode mode)
{
    this->c6xx_controller = c6xx_controller;
    this->id = id;
    this->mode = mode;
    speed_pid = std::make_shared<PID>();
    pos_pid = std::make_shared<PID>(speed_pid);
    pos_pid->set_pid_type(PID::PIDType::Pos);
    set_max_output_current(10);

    last_rotor_pos = get_rotor_pos();
}

DjiMotor::~DjiMotor()
{

}

/*
 * @brief 获取电机转子转速
 */
double DjiMotor::get_rotor_rpm()
{
    rotor_rpm = c6xx_controller->read_rpm(id);
    return rotor_rpm;
}

/*
 * @brief 获取电机转子位置
 */
double DjiMotor::get_rotor_pos()
{
    rotor_pos = c6xx_controller->read_angle(id);
    return rotor_pos;
}

/*
 * @brief 计算电机位置
 */
void DjiMotor::calc_pos()
{
    double offset = rotor_pos - last_rotor_pos;
    if (offset < -4096) { //正转，零点在区间内
        pos += 8192 + offset;
    } else if (offset >= -4096 && offset < 0) { //反转，零点不在区间内
        pos += offset;
    } else if (offset >= 0 && offset < 4096) { //正转，零点不在区间内
        pos += offset;
    } else { //反转，零点在区间内
        pos += offset - 8192;
    }
    last_rotor_pos = rotor_pos;
}

void DjiMotor::speed_mode_tick()
{
    get_rotor_rpm();
    get_rotor_pos();
    calc_pos();

    speed_pid->set_input(rotor_rpm);
    speed_pid->set_target(target_rpm);
    speed_pid->tick();

    c6xx_controller->write_current(id, speed_pid->get_output());
}

void DjiMotor::speed_pos_mode_tick()
{
    get_rotor_rpm();
    get_rotor_pos();
    calc_pos();

    speed_pid->set_input(rotor_rpm);
    speed_pid->set_target(pos_pid->get_output());
    pos_pid->set_input(DjiMotor::get_pos());
    pos_pid->set_target(target_pos);
    pos_pid->tick();

    c6xx_controller->write_current(id, speed_pid->get_output());
}

/*
 * @brief 每隔1ms调用一次
 */
void DjiMotor::tick()
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
 * @brief 设置电机最大输出电流
 * @param current 电流(A)
 * @attention 电流设置过大可能会烧毁电机或者出现电机疯转停不下来的风险
 */
void DjiMotor::set_max_output_current(double current)
{
    if (current < 0) current = -current;
    max_output_current = current / 20.0 * 32768;
    set_speed_pid_output_range(-max_output_current, max_output_current);
}

/*
 * @brief 设置转速
 * @param rpm 转速
 */
void DjiMotor::set_target_rpm(double target_rpm)
{
    if (reverse) this->target_rpm = -target_rpm;
    else this->target_rpm = target_rpm;
    double abs_rpm = abs(target_rpm);
    set_pos_pid_output_range(-abs_rpm, abs_rpm);
}

/*
 * @brief 设置位置
 * @param angle 角度
 */
void DjiMotor::set_target_pos(double target_pos)
{
    if (reverse) this->target_pos = -target_pos;
    else this->target_pos = target_pos;
}

/*
 * @brief 获取电机转速
 */
double DjiMotor::get_rpm()
{
    if (reverse) return -rotor_rpm;
    return rotor_rpm;
}

/*
 * @brief 获取电机位置
 */
double DjiMotor::get_pos()
{
    return pos;
}

/*
 * @brief 设置速度PID
 * @param kp p比例系数
 * @param ki i比例系数
 * @param kd d比例系数
 */
void DjiMotor::set_speed_pid(double kp, double ki, double kd)
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
void DjiMotor::set_pos_pid(double kp, double ki, double kd)
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
void DjiMotor::set_pos_pid_total_error_range(double min, double max)
{
    if (pos_pid) {
        pos_pid->set_total_error_range(min, max);
    }
}

/*
 * @brief 设置速度PID输出电流范围
 * @param min 最低值
 * @param max 最高值
 */
void DjiMotor::set_speed_pid_output_range(double min, double max)
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
void DjiMotor::set_pos_pid_output_range(double min, double max)
{
    if (pos_pid) {
        pos_pid->set_output_range(min, max);
    }
}

/*
 * @brief 设置电机是否反转
 * @param reverse 是否反转
 */
void DjiMotor::set_reverse(bool reverse)
{
    this->reverse = reverse;
}