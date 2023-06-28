#include "pid.h"


namespace RDK
{



/*
 * @brief 增量式算法
 */
void PID::IncTick()
{
	this->lastError2 = this->lastError;
    this->lastError = this->error;
    this->error = this->target - this->input;

    double output = this->kp * (this->error - this->lastError) +
                 this->ki * this->error +
                 this->kd * (this->error - 2 * this->lastError + this->lastError2);
    this->output += output;
    this->output = CheckRange(this->output, GetOutputRange());
}

/*
 * @brief 位置式算法
 */
void PID::PosTick()
{
	this->lastError = this->error;
    this->error = this->target - this->input;
    this->totalError += this->error;
	this->totalError = CheckRange(this->totalError, GetTotalErrorRange());

    double output = this->kp * this->error +
                 this->ki * this->totalError +
                 this->kd * (this->error - this->lastError);
    this->output = output;
    this->output = CheckRange(this->output, GetOutputRange());
}

/*
 * @brief 范围检查
 * @param value 输入值
 * @param range 范围
 */
double PID::CheckRange(double value, const std::pair<double, double>& range)
{
	if (range.first == range.second) return value; //相等则不进行范围检查
	if (value < range.first) value = range.first;
	if (value > range.second) value = range.second;
	return value;
}

/*
 * @brief 默认初始化，增量式，不限幅
 */
PID::PID()
{
	
}

/*
 * @brief 带有pid算法类似和比例系数参数的初始化
 */
PID::PID(PIDType pidtype, double kp, double ki, double kd)
{
	this->pidType = pidtype;
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
}

/*
 * @brief 目前没有啥可清理的
 */
PID::~PID()
{

}
	
/*
 * @brief 设置PID类型
 * @param pidType PID类型(增量式/位置式)
 */
PIDType PID::SetPIDType(PIDType pidType)
{
	return this->pidType = pidType;
}

/*
 * @brief 获取PID类型
 */
PIDType PID::GetPIDType()
{
	return this->pidType;
}

/*
 * @brief 设置PID类型
 * @param kp p比例系数
 */
double PID::SetKp(double kp)
{
	return this->kp = kp;
}

/*
 * @brief 获取p比例系数
 */
double PID::GetKp()
{
	return this->kp;
}

/*
 * @brief 设置i比例系数
 * @param ki i比例系数
 */
double PID::SetKi(double ki)
{
	return this->ki = ki;
}

/*
 * @brief 获取i比例系数
 */
double PID::GetKi()
{
	return this->ki;
}

/*
 * @brief 设置d比例系数
 * @param kd d比例系数
 */
double PID::SetKd(double kd)
{
	return this->kd = kd;
}

/*
 * @brief 获取d比例系数
 */
double PID::GetKd()
{
	return this->kd;
}

/*
 * @brief 设置输入值
 * @param input 输入值
 */
double PID::SetInput(double input)
{
	return this->input = input;
}

/*
 * @brief 获取输入值
 */
double PID::GetInput()
{
	return this->input;
}

/*
 * @brief 设置目标值
 * @param target 目标值
 */
double PID::SetTarget(double target)
{
	return this->target = target;
}

/*
 * @brief 获取目标值
 */
double PID::GetTarget()
{
	return this->target;
}

/*
 * @brief 获取输出值
 */
double PID::GetOutput()
{
	return this->output;
}

/*
 * @brief 设置总误差范围
 * @param min 最小值
 * @param max 最大值
 */
std::pair<double, double> PID::SetTotalErrorRange(double min, double max)
{
	return this->totalErrorRange = std::make_pair(min, max);
}

/*
 * @brief 设置总误差范围
 * @param range 范围
 */
std::pair<double, double> PID::SetTotalErrorRange(const std::pair<double, double>& range)
{
	return this->totalErrorRange = range;
}

/*
 * @brief 获取总误差范围
 */
std::pair<double, double> PID::GetTotalErrorRange()
{
	return this->totalErrorRange;
}

/*
 * @brief 设置输出值范围
 * @param min 最小值
 * @param max 最大值
 */
std::pair<double, double> PID::SetOutputRange(double min, double max)
{
	return this->outputRange = std::make_pair(min, max);
}

/*
 * @brief 设置输出值范围
 * @param range 范围
 */
std::pair<double, double> PID::SetOutputRange(const std::pair<double, double>& range)
{
	return this->outputRange = range;
}

/*
 * @brief 获取输出值范围
 */
std::pair<double, double> PID::GetOutputRange()
{
	return this->outputRange;
}

/*
 * @brief 调用pid算法进行一次计算
 */
double PID::Tick()
{
	switch (pidType)
	{
	case PIDType::Inc:
		IncTick();
		break;
	case PIDType::Pos:
		PosTick();
		break;
	default:
		break;
	}
	return GetOutput();
}


}

