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

PID::PIDType PID::set_pid_type(PIDType pid_type)
{
    return this->pid_type = pid_type;
}

PID::PIDType PID::get_pid_type()
{
    return pid_type;
}

double PID::set_kp(double kp)
{
    return this->kp = kp;
}

double PID::get_kp()
{
    return kp;
}

double PID::set_ki(double ki)
{
    return this->ki = ki;
}

double PID::get_ki()
{
    return ki;
}

double PID::set_kd(double kd)
{
    return this->kd = kd;
}

double PID::get_kd()
{
    return kd;
}

double PID::set_input(double input)
{
    return this->input = input;
}

double PID::get_input()
{
    return input;
}

double PID::set_target(double target)
{
    return this->target = target;
}

double PID::get_target()
{
    return target;
}

double PID::get_output()
{
    return output;
}

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

std::pair<double, double> PID::set_total_error_range(double min, double max)
{
    return this->total_error_range = std::make_pair(min, max);
}

std::pair<double, double> PID::set_total_error_range(const std::pair<double, double>& range)
{
    return this->total_error_range = range;
}

std::pair<double, double> PID::get_total_error_range()
{
    return total_error_range;
}

std::pair<double, double> PID::set_output_range(double min, double max)
{
    return this->output_range = std::make_pair(min, max);
}

std::pair<double, double> PID::set_output_range(const std::pair<double, double>& range)
{
    return this->output_range = range;
}

std::pair<double, double> PID::get_output_range()
{
    return this->output_range;
}

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

double PID::check_range(double value, const std::pair<double, double>& range)
{
    if (range.first == range.second) return value; //相等则不进行范围检查
    if (value < range.first) value = range.first;
    if (value > range.second) value = range.second;
    return value;
}