/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>
#include <utility>
#include <memory>

class PID
{
public:

    enum class PIDType
    {
        Inc = 1, //增量式
        Pos = 2, //位置式
    };

    PID();
    PID(std::shared_ptr<PID> inner_pid);
    ~PID();
    PIDType set_pid_type(PIDType pid_type);
    PIDType get_pid_type();
    double set_kp(double kp);
    double get_kp();
    double set_ki(double ki);
    double get_ki();
    double set_kd(double kd);
    double get_kd();
    double set_input(double input);
    double get_input();
    double set_target(double target);
    double get_target();
    double get_output();
    double tick();
    std::pair<double, double> set_total_error_range(double min, double max);
    std::pair<double, double> set_total_error_range(const std::pair<double, double>& range);
    std::pair<double, double> get_total_error_range();
    std::pair<double, double> set_output_range(double min, double max);
    std::pair<double, double> set_output_range(const std::pair<double, double>& range);
    std::pair<double, double> get_output_range();

private:
    std::shared_ptr<PID> inner_pid;
    int tick_cnt = 0;
    PIDType pid_type = PIDType::Inc;
    double kp = 0; //p比例系数
    double ki = 0; //i比例系数
    double kd = 0; //d比例系数
    volatile double input; //输入值
    volatile double target; //目标值
    volatile double error; //误差（目标值-输入值）
    volatile double total_error; //总误差， 位置式算法中使用此变量
    volatile double last_error; //上一次误差
    volatile double last_error2; //上上一次误差， 增量式算法中使用此变量
    volatile double output;
    std::pair<double, double> total_error_range = {0, 0}; //限制积分幅度
    std::pair<double, double> output_range = {0, 0}; //限制输出幅度
    void inc_tick();
    void pos_tick();
    double check_range(double value, const std::pair<double, double>& range);

};