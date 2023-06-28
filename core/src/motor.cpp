#include "motor.h"


namespace RDK
{


/*
 * @brief 编码电机初始化，使用增量式PID算法
 */
EncodingMotor::EncodingMotor()
{
    pid.SetPIDType(RDK::PIDType::Inc);
}

/*
 * @brief 暂时为空
 */
EncodingMotor::~EncodingMotor()
{

}

/*
 * @brief 设置编码电机的PID参数
 * @param p p比例系数
 * @param i i比例系数
 * @param d d比例系数
 */
void EncodingMotor::SetPID(double p, double i, double d)
{
    pid.SetKp(p);
    pid.SetKi(i);
    pid.SetKd(d);
}

/*
 * @brief 设置编码电机的PWM输出范围
 * @param min 最小值
 * @param max 最大值
 */
void EncodingMotor::SetOutputRange(double min, double max)
{
    pid.SetOutputRange(min, max);
}

/*
 * @brief 设置编码电机是否反转
 * @param reverse 是否反转
 */
void EncodingMotor::SetReverse(bool reverse)
{
    this->reverse = reverse;
}

/*
 * @brief 设置编码电机的转速
 * @param speed 转速
 */
void EncodingMotor::SetSpeed(double speed)
{
    if (this->reverse) speed = -speed;
    pid.SetTarget(speed);
}

/*
 * @brief 设置电机PWM输出回调函数
 * @param motorCallback 
 * @details 用户应在motorCallback中将PWM值输出到电机
 */
void EncodingMotor::SetCallback(MotorCallback motorCallback)
{
    this->motorCallback = motorCallback;
}

/*
 * @brief 增加脉冲数
 * @param pulse 脉冲数
 * @attention 必须每次在Tick之前调用此方法，将传感器检测到的转速调递给PID算法
 */
int EncodingMotor::AddPulse(int pulse)
{
    pid.SetInput(pulse);
    if (reverse) pulse = -pulse;
    return this->pulse += pulse;
}

/*
 * @brief 获取电机的脉冲数
 */
int EncodingMotor::GetPulse()
{
    return this->pulse;
}

/*
 * @brief 清除脉冲数
 */
int EncodingMotor::ClearPulse()
{
    return this->pulse = 0;
}

/*
 * @brief 进行一次PID运算，并输出PWM到电机
 */
void EncodingMotor::Tick()
{
    pid.Tick();
    if (motorCallback) motorCallback(pid.GetOutput());
}






}