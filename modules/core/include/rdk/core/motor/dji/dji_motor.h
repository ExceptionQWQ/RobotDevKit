/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
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

    /*
     * @brief 每隔1ms调用一次
     */
    void tick() override;
    /*
     * @brief 设置电机最大输出电流
     * @param current 电流(A)
     * @attention 电流设置过大可能会烧毁电机或者出现电机疯转停不下来的风险
     */
    virtual void set_max_output_current(double current);
    /*
     * @brief 设置转速
     * @param rpm 转速
     */
    void set_target_rpm(double rpm) override;
    /*
     * @brief 设置位置
     * @param angle 角度
     */
    virtual void set_target_pos(double target_pos);
    /*
     * @brief 获取电机转速
     */
    double get_rpm() override;
    /*
     * @brief 获取电机位置
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
     * @brief 设置电机是否反转
     * @param reverse 是否反转
     */
    void set_reverse(bool reverse) override;

protected:
    double max_output_current = 0;
    /*
     * @brief 设置速度PID输出电流范围
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


private:
    int id;
    std::shared_ptr<C6xxController> c6xx_controller;
    Mode mode;
    bool reverse = false;

    double target_rpm = 0;
    double target_pos = 0;
    double rotor_rpm = 0;
    double rotor_pos = 0;
    double last_rotor_pos = 0;
    double pos = 0;

    std::shared_ptr<PID> speed_pid;
    std::shared_ptr<PID> pos_pid;

    void speed_mode_tick();
    void speed_pos_mode_tick();
    /*
     * @brief 获取电机转子转速
     */
    double get_rotor_rpm();
    /*
     * @brief 获取电机转子位置
     */
    double get_rotor_pos();
    /*
     * @brief 计算电机位置
     */
    void calc_pos();

};