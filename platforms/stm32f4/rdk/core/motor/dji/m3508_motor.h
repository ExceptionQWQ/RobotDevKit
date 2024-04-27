/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 */

#pragma once

#include "rdk/core/motor/dji/dji_motor.h"


class M3508Motor : public DjiMotor
{
public:
    M3508Motor(std::shared_ptr<C6xxController> c6xx_controller, int id, Mode mode = Mode::SPEED);
    ~M3508Motor();

    void set_target_rpm(double rpm) override;
    void set_target_pos(double target_pos) override;
    double get_rpm() override;
    double get_pos() override;
    void set_max_output_current(double current) override;

private:
    double reduction_ratio = 19.0;

};
