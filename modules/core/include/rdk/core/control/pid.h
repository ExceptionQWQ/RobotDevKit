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
    /*
     * @brief 带有内环pid的构造，tick当前pid会自动tick内环pid
     */
    PID(std::shared_ptr<PID> inner_pid);
    ~PID();
    /*
     * @brief 设置PID类型
     * @param pid_type pid_type
     * @details 可以设置的PID类型有增量式PID和位置式PID，取值分别为PIDType::Inc和PIDType::Pos
     */
    PIDType set_pid_type(PIDType pid_type);
    /*
     * @brief 获取PID类型
     * @return PIDType
     */
    PIDType get_pid_type();
    /*
     * @brief 设置kP系数
     * @param kP
     */
    double set_kp(double kp);
    /*
     * @brief 获取kP系数
     */
    double get_kp();
    /*
     * @brief 设置kI系数
     * @param kI
     */
    double set_ki(double ki);
    /*
     * @brief 获取kI系数
     */
    double get_ki();
    /*
     * @brief 设置kD系数
     * @param kD
     */
    double set_kd(double kd);
    /*
     * @brief 获取kD系数
     */
    double get_kd();
    /*
     * @brief 设置PID算法输入量
     * @param input
     */
    double set_input(double input);
    /*
     * @brief 获取PID算法输入量
     */
    double get_input();
    /*
     * @brief 设置PID算法目标值
     * @param target
     */
    double set_target(double target);
    /*
     * @brief 获取PID算法目标值
     */
    double get_target();
    /*
     * @brief 获取PID算法输出值
     */
    double get_output();
    /*
     * @brief 每隔一段时间调用一次tick计算输出量，例如每隔1ms tick一次
     */ 
    virtual double tick();
    /*
     * @brief 设置积分误差范围
     * @param min
     * @param max
     * @details 积分误差将会被限制在min到max的范围内
     */
    std::pair<double, double> set_total_error_range(double min, double max);
    /*
     * @brief 设置积分误差范围
     * @param range
     * @details 积分误差将会被限制在range的范围内
     */
    std::pair<double, double> set_total_error_range(const std::pair<double, double>& range);
    /*
     * @brief 获取积分误差范围
     * @return range
     */
    std::pair<double, double> get_total_error_range();
    /*
     * @brief 设置输出值范围
     * @param min
     * @param max
     * @details 输出值将会被限制在min到max的范围内
     */
    std::pair<double, double> set_output_range(double min, double max);
    /*
     * @brief 设置输出值范围
     * @param range
     * @details 输出值将会被限制在range的范围内
     */
    std::pair<double, double> set_output_range(const std::pair<double, double>& range);
    /*
     * @brief 获取输出值范围
     * @return range
     */
    std::pair<double, double> get_output_range();

protected:
    std::shared_ptr<PID> inner_pid;
    int tick_cnt = 0;
    PIDType pid_type = PIDType::Inc;
    double kp = 0; //p比例系数
    double ki = 0; //i比例系数
    double kd = 0; //d比例系数
    double input; //输入值
    double target; //目标值
    double error; //误差（目标值-输入值）
    double total_error; //总误差， 位置式算法中使用此变量
    double last_error; //上一次误差
    double last_error2; //上上一次误差， 增量式算法中使用此变量
    double output;
    std::pair<double, double> total_error_range = {0, 0}; //限制积分幅度
    std::pair<double, double> output_range = {0, 0}; //限制输出幅度
    /*
     * @brief 增量式算法tick
     */
    void inc_tick();
    /*
     * @brief 位置式算法tick
     */
    void pos_tick();
    /*
     * @brief 范围约束
     * @param value
     * @param range
     * @details 将value约束在range范围内，如果range为空，则不进行约束
     */
    double check_range(double value, const std::pair<double, double>& range);

};