/*
 * @author BusyBox
 * @date 2024/5/4
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cmath>
#include "rdk/core/control/pid.h"

class RingPID : public PID
{
public:
    RingPID();
    ~RingPID();

    /*
     * @brief 设置输入值区间范围[min, max]
     * @param min 区间下限
     * @param max 区间上限
     */
    void set_input_range(double min, double max);

    double tick() override;


private:
    std::pair<double, double> input_range;

};