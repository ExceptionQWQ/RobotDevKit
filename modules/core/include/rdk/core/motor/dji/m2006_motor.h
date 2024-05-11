/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include "rdk/core/motor/dji/dji_motor.h"


class M2006Motor : public DjiMotor
{
public:
    M2006Motor(std::shared_ptr<C6xxController> c6xx_controller, int id, Mode mode = Mode::SPEED);
    ~M2006Motor();

    /*
     * @brief 设置目标转速
     * @param rpm 转速
     */
    void set_target_rpm(double rpm) override;
    /*
     * @brief 设置目标位置
     * @param target_pos 目标位置，角度
     */
    void set_target_pos(double target_pos) override;
    /*
     * @brief 获取转速
     * @return rpm
     */
    double get_rpm() override;
    /*
     * @brief 获取位置
     * @return 角度
     */
    double get_pos() override;
    /*
     * @brief 设置电机最大输出电流
     * @param current 电流(A)
     * @attention 电流设置过大可能会烧毁电机或者出现电机疯转停不下来的风险
     */
    void set_max_output_current(double current) override;

private:
    double reduction_ratio = 36.0;

};