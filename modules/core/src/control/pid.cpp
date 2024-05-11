/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/control/pid.h"

PID::PID()
{

}

/*
 * @brief 带有内环pid的构造，tick当前pid会自动tick内环pid
 */
PID::PID(std::shared_ptr<PID> inner_pid)
{
    this->inner_pid = inner_pid;
}

PID::~PID()
{

}

/*
 * @brief 设置PID类型
 * @param pid_type pid_type
 * @details 可以设置的PID类型有增量式PID和位置式PID，取值分别为PIDType::Inc和PIDType::Pos
 */
PID::PIDType PID::set_pid_type(PIDType pid_type)
{
    return this->pid_type = pid_type;
}

/*
 * @brief 获取PID类型
 * @return PIDType
 */
PID::PIDType PID::get_pid_type()
{
    return pid_type;
}

/*
 * @brief 设置kP系数
 * @param kP
 */
double PID::set_kp(double kp)
{
    return this->kp = kp;
}

/*
 * @brief 获取kP系数
 */
double PID::get_kp()
{
    return kp;
}

/*
 * @brief 设置kI系数
 * @param kI
 */
double PID::set_ki(double ki)
{
    return this->ki = ki;
}

/*
 * @brief 获取kI系数
 */
double PID::get_ki()
{
    return ki;
}

/*
 * @brief 设置kD系数
 * @param kD
 */
double PID::set_kd(double kd)
{
    return this->kd = kd;
}

/*
 * @brief 获取kD系数
 */
double PID::get_kd()
{
    return kd;
}

/*
 * @brief 设置PID算法输入量
 * @param input
 */
double PID::set_input(double input)
{
    return this->input = input;
}

/*
 * @brief 获取PID算法输入量
 */
double PID::get_input()
{
    return input;
}

/*
 * @brief 设置PID算法目标值
 * @param target
 */
double PID::set_target(double target)
{
    return this->target = target;
}

/*
 * @brief 获取PID算法目标值
 */
double PID::get_target()
{
    return target;
}

/*
 * @brief 获取PID算法输出值
 */
double PID::get_output()
{
    return output;
}

/*
 * @brief 每隔一段时间调用一次tick计算输出量，例如每隔1ms tick一次
 */
double PID::tick()
{
    ++tick_cnt;
    if (inner_pid) {
        inner_pid->tick();
        if (tick_cnt % 10 == 0) {
            switch (pid_type)
            {
                case PIDType::Inc:
                    inc_tick();
                    break;
                case PIDType::Pos:
                    pos_tick();
                    break;
                default:
                    break;
            }
        }
    } else {
        switch (pid_type)
        {
            case PIDType::Inc:
                inc_tick();
                break;
            case PIDType::Pos:
                pos_tick();
                break;
            default:
                break;
        }
    }
    return get_output();
}

/*
 * @brief 设置积分误差范围
 * @param min
 * @param max
 * @details 积分误差将会被限制在min到max的范围内
 */
std::pair<double, double> PID::set_total_error_range(double min, double max)
{
    return this->total_error_range = std::make_pair(min, max);
}

/*
 * @brief 设置积分误差范围
 * @param range
 * @details 积分误差将会被限制在range的范围内
 */
std::pair<double, double> PID::set_total_error_range(const std::pair<double, double>& range)
{
    return this->total_error_range = range;
}

/*
 * @brief 获取积分误差范围
 * @return range
 */
std::pair<double, double> PID::get_total_error_range()
{
    return total_error_range;
}

/*
 * @brief 设置输出值范围
 * @param min
 * @param max
 * @details 输出值将会被限制在min到max的范围内
 */
std::pair<double, double> PID::set_output_range(double min, double max)
{
    return this->output_range = std::make_pair(min, max);
}

/*
 * @brief 设置输出值范围
 * @param range
 * @details 输出值将会被限制在range的范围内
 */
std::pair<double, double> PID::set_output_range(const std::pair<double, double>& range)
{
    return this->output_range = range;
}

/*
 * @brief 获取输出值范围
 * @return range
 */
std::pair<double, double> PID::get_output_range()
{
    return this->output_range;
}

/*
 * @brief 增量式算法tick
 */
void PID::inc_tick()
{
    this->last_error2 = this->last_error;
    this->last_error = this->error;
    this->error = get_target() - get_input();

    double output = this->kp * (this->error - this->last_error) +
                    this->ki * this->error +
                    this->kd * (this->error - 2 * this->last_error + this->last_error2);
    this->output += output;
    this->output = check_range(this->output, get_output_range());
}

/*
 * @brief 位置式算法tick
 */
void PID::pos_tick()
{
    this->last_error = this->error;
    this->error = get_target() - get_input();
    this->total_error += this->error;
    this->total_error = check_range(this->total_error, get_total_error_range());

    double output = this->kp * this->error +
                    this->ki * this->total_error +
                    this->kd * (this->error - this->last_error);
    this->output = output;
    this->output = check_range(this->output, get_output_range());
}

/*
 * @brief 范围约束
 * @param value
 * @param range
 * @details 将value约束在range范围内，如果range为空，则不进行约束
 */
double PID::check_range(double value, const std::pair<double, double>& range)
{
    if (range.first == range.second) return value; //相等则不进行范围检查
    if (value < range.first) value = range.first;
    if (value > range.second) value = range.second;
    return value;
}