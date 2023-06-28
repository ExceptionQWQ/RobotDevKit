#ifndef PID_H
#define PID_H

#ifdef __cplusplus

#include <utility>

namespace RDK
{


enum class PIDType
{
    Inc = 1, //增量式
    Pos = 2, //位置式
};


class PID
{
private:
    PIDType pidType = PIDType::Inc;
    double kp = 0; //p比例系数
    double ki = 0; //i比例系数
    double kd = 0; //d比例系数
    volatile double input; //输入值
    volatile double target; //目标值
    volatile double error; //误差（目标值-输入值）
    volatile double totalError; //总误差， 位置式算法中使用此变量
    volatile double lastError; //上一次误差
    volatile double lastError2; //上上一次误差， 增量式算法中使用此变量
    volatile double output;
    std::pair<double, double> totalErrorRange = {0, 0}; //限制积分幅度
    std::pair<double, double> outputRange = {0, 0}; //限制输出幅度
    void IncTick();
    void PosTick();
    double CheckRange(double value, const std::pair<double, double>& range);
public:
    PID();
    PID(PIDType pidtype, double kp, double ki, double kd);
    ~PID();
    PIDType SetPIDType(PIDType pidType);
    PIDType GetPIDType();
    double SetKp(double kp);
    double GetKp();
    double SetKi(double ki);
    double GetKi();
    double SetKd(double kd);
    double GetKd();
    double SetInput(double input);
    double GetInput();
    double SetTarget(double target);
    double GetTarget();
    double GetOutput();
    std::pair<double, double> SetTotalErrorRange(double min, double max);
    std::pair<double, double> SetTotalErrorRange(const std::pair<double, double>& range);
    std::pair<double, double> GetTotalErrorRange();
    std::pair<double, double> SetOutputRange(double min, double max);
    std::pair<double, double> SetOutputRange(const std::pair<double, double>& range);
    std::pair<double, double> GetOutputRange();
    double Tick();
};


}

#endif






#endif