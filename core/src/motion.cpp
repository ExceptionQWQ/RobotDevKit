#include "motion.h"

namespace RDK
{

	MecanumMotion::MecanumMotion()
	{
		double wheelDirectionVectorLen = std::sqrt(2);
		wheelADirectionVector.first /= wheelDirectionVectorLen;
		wheelADirectionVector.second /= wheelDirectionVectorLen;
		wheelBDirectionVector.first /= wheelDirectionVectorLen;
		wheelBDirectionVector.second /= wheelDirectionVectorLen;
	}

	MecanumMotion::~MecanumMotion()
	{
	}

	void MecanumMotion::SetEncodingMotor1(EncodingMotor *motor)
	{
		this->motor1 = motor;
	}

	void MecanumMotion::SetEncodingMotor2(EncodingMotor *motor)
	{
		this->motor2 = motor;
	}

	void MecanumMotion::SetEncodingMotor3(EncodingMotor *motor)
	{
		this->motor3 = motor;
	}

	void MecanumMotion::SetEncodingMotor4(EncodingMotor *motor)
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
		if (motor1)
			motor1->SetSpeed(motor1Speed);
		if (motor2)
			motor2->SetSpeed(motor2Speed);
		if (motor3)
			motor3->SetSpeed(motor3Speed);
		if (motor4)
			motor4->SetSpeed(motor4Speed);
	}

	void MecanumMotion::SetPosPID(double p, double i, double d)
	{
		this->kp = p;
		this->ki = i;
		this->kd = d;
	}

	/*
	 * @brief 2维平面内上下左右移动
	 * @attention 调用此方法会一直阻塞到移动结束！
	 * @param speed 移动速度
	 * @param xDis x轴移动距离
	 * @param yDis y轴移动距离
	 */
	void MecanumMotion::Move(double speed, double xDis, double yDis)
	{
		PID motor1PosPID(RDK::PIDType::Pos, this->kp, this->ki, this->kd);
		PID motor2PosPID(RDK::PIDType::Pos, this->kp, this->ki, this->kd);
		PID motor3PosPID(RDK::PIDType::Pos, this->kp, this->ki, this->kd);
		PID motor4PosPID(RDK::PIDType::Pos, this->kp, this->ki, this->kd);
		if (speed < 0)
			speed = -speed;
		motor1PosPID.SetTotalErrorRange(minTotalError, maxTotalError);
		motor2PosPID.SetTotalErrorRange(minTotalError, maxTotalError);
		motor3PosPID.SetTotalErrorRange(minTotalError, maxTotalError);
		motor4PosPID.SetTotalErrorRange(minTotalError, maxTotalError);
		motor1PosPID.SetOutputRange(-speed, speed);
		motor2PosPID.SetOutputRange(-speed, speed);
		motor3PosPID.SetOutputRange(-speed, speed);
		motor4PosPID.SetOutputRange(-speed, speed);

		double motor1TargetDis = xDis * wheelADirectionVector.first + yDis * wheelADirectionVector.second;
		double motor2TargetDis = xDis * wheelBDirectionVector.first + yDis * wheelBDirectionVector.second;
		double motor3TargetDis = xDis * wheelBDirectionVector.first + yDis * wheelBDirectionVector.second;
		double motor4TargetDis = xDis * wheelADirectionVector.first + yDis * wheelADirectionVector.second;

		motor1->ClearPulse();
		motor2->ClearPulse();
		motor3->ClearPulse();
		motor4->ClearPulse();

		motor1PosPID.SetTarget(motor1TargetDis);
		motor2PosPID.SetTarget(motor2TargetDis);
		motor3PosPID.SetTarget(motor3TargetDis);
		motor4PosPID.SetTarget(motor4TargetDis);

		while (true)
		{
			if (std::fabs(motor1->GetPulse() - motor1TargetDis) < 5 &&
				std::fabs(motor2->GetPulse() - motor2TargetDis) < 5 &&
				std::fabs(motor3->GetPulse() - motor3TargetDis) < 5 &&
				std::fabs(motor4->GetPulse() - motor4TargetDis) < 5)
				break;
			motor1PosPID.SetInput(motor1->GetPulse());
			motor1PosPID.Tick();
			motor2PosPID.SetInput(motor2->GetPulse());
			motor2PosPID.Tick();
			motor3PosPID.SetInput(motor3->GetPulse());
			motor3PosPID.Tick();
			motor4PosPID.SetInput(motor4->GetPulse());
			motor4PosPID.Tick();

			ClearSpeed();
			motor1Speed += motor1PosPID.GetOutput();
			motor2Speed += motor2PosPID.GetOutput();
			motor3Speed += motor3PosPID.GetOutput();
			motor4Speed += motor4PosPID.GetOutput();
			CommitSpeed();
		}
		ClearSpeed();
		CommitSpeed();
	}

}
