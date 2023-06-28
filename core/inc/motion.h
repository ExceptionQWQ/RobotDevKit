#ifndef MOTION_H
#define MOTION_H

#include "motor.h"



#ifdef __cplusplus


namespace RDK
{


/*
 * @brief 4个麦克纳姆轮(ABBA)
 * 
 *  |-----------------------|
 *  | motor1(A)   motor2(B) |
 *  |                       | 
 *  |                       |
 *  |                       | 
 *  |                       |
 *  |                       | 
 *  |                       |
 *  |                       | 
 *  |                       |
 *  | motor3(B)   motor4(A) |
 *  |-----------------------|    
 *           
 *  小车前方为x轴，左边为y轴
 */
class MecanumMotion
{
private:
    EncodingMotor* motor1 = nullptr;
    EncodingMotor* motor2 = nullptr;
    EncodingMotor* motor3 = nullptr;
    EncodingMotor* motor4 = nullptr;
    double motor1Speed = 0;
    double motor2Speed = 0;
    double motor3Speed = 0;
    double motor4Speed = 0;
public:
    MecanumMotion();
    ~MecanumMotion();
    void SetEncodingMotor1(EncodingMotor* motor);
    void SetEncodingMotor2(EncodingMotor* motor);
    void SetEncodingMotor3(EncodingMotor* motor);
    void SetEncodingMotor4(EncodingMotor* motor);
    void ClearSpeed();
    void AddXSpeed(double speed);
    void AddYSpeed(double speed);
    void AddZSpeed(double speed);
    void CommitSpeed();
};


}


#endif








#endif