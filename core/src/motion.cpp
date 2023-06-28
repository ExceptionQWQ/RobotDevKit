#include "motion.h"

namespace RDK
{
	

MecanumMotion::MecanumMotion()
{

}

MecanumMotion::~MecanumMotion()
{

}

void MecanumMotion::SetEncodingMotor1(EncodingMotor* motor)
{
	this->motor1 = motor;
}

void MecanumMotion::SetEncodingMotor2(EncodingMotor* motor)
{
	this->motor2 = motor;
}

void MecanumMotion::SetEncodingMotor3(EncodingMotor* motor)
{
	this->motor3 = motor;
}

void MecanumMotion::SetEncodingMotor4(EncodingMotor* motor)
{
	this->motor4 = motor;
}

void MecanumMotion::ClearSpeed()
{
	motor1Speed = 0;
	motor2Speed = 0;
	motor3Speed = 0;
	motor4Speed = 0;
}

void MecanumMotion::AddXSpeed(double speed)
{
	motor1Speed += speed;
	motor2Speed += speed;
	motor3Speed += speed;
	motor4Speed += speed;
}

void MecanumMotion::AddYSpeed(double speed)
{
	motor1Speed -= speed;
	motor2Speed += speed;
	motor3Speed += speed;
	motor4Speed -= speed;
}

void MecanumMotion::AddZSpeed(double speed)
{
	motor1Speed -= speed;
	motor2Speed += speed;
	motor3Speed -= speed;
	motor4Speed += speed;
}

void MecanumMotion::CommitSpeed()
{
	if (motor1) motor1->SetSpeed(motor1Speed);
	if (motor2) motor2->SetSpeed(motor2Speed);
	if (motor3) motor3->SetSpeed(motor3Speed);
	if (motor4) motor4->SetSpeed(motor4Speed);
}






}
