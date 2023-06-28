#ifndef MOTOR_H
#define MOTOR_H

#include "pid.h"


#ifdef __cplusplus

#include <utility>

namespace RDK
{

using MotorCallback = void(*)(double pwm);

class EncodingMotor
{
private:
    bool reverse = false; //是否反转
    volatile int pulse = 0; //脉冲数
    MotorCallback motorCallback = nullptr; //电机PWM输出回调函数
    PID pid;
public:
    EncodingMotor();
    ~EncodingMotor();
    void SetPID(double p, double i, double d);
    void SetOutputRange(double min, double max);
    void SetReverse(bool reverse);
    void SetSpeed(double speed);
    void SetCallback(MotorCallback motorCallback);
    int AddPulse(int pulse);
    int GetPulse();
    int ClearPulse();
    void Tick();
};



}

#endif






#endif