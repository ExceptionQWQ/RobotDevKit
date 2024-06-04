/*
 * @author BusyBox
 * @date 2024/5/4
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/control/ring_pid.h"

RingPID::RingPID()
{

}

RingPID::~RingPID()
{

}

/*
 * @brief 设置输入值区间范围[min, max]
 * @param min 区间下限
 * @param max 区间上限
 */
void RingPID::set_input_range(double min, double max)
{
    input_range = std::make_pair(min, max);
}

double RingPID::tick()
{
    input = check_range(input, input_range);

    /*
     * 算法思路：将输入区间[min, max]扩容，左右两边各自加一个与输入区间一样大小的区间
     * 扩容后的区间：[2 * min - max, 2 * max - min], 即[min - size, max + size]
     * 再根据位于中间那个区间的target点计算出这个target点在左右两边区间的等效点，这样就相当于在一条直线上有3个target点
     */
    double target1 = get_target() - (input_range.second - input_range.first);
    double target2 = get_target();
    double target3 = get_target() + (input_range.second - input_range.first);

    double dis1 = std::fabs(target1 - input);
    double dis2 = std::fabs(target2 - input);
    double dis3 = std::fabs(target3 - input);

    //然后再判断input点距离哪个target点最近，再将最近的target点传递给PID算法
    if (dis1 <= dis2 && dis1 <= dis3) PID::set_target(target1);
    else if (dis2 <= dis1 && dis2 <= dis3) PID::set_target(target2);
    else PID::set_target(target3);

    return PID::tick();
}