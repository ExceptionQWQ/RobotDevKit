/*
 * @author BusyBox
 * @date 2024/5/26
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>
#include <memory>
#include <thread>
#include <functional>
#include <chrono>
#include <cmath>
#include <string>
#include "rdk/core/transfer/io_stream.h"

class FeetechProtocol
{
public:
    FeetechProtocol(std::shared_ptr<IOStream> io_stream);
    ~FeetechProtocol();


private:
    std::shared_ptr<IOStream> io_stream;
    int timeout = 10;

    /*
     * 飞特串行总线智能舵机通讯协议指令类型
     */
    enum class ControlCMD : uint8_t
    {
        Ping = 1,  //（查询） 查询工作状态
        ReadData = 2, //（读） 查询控制表里的字符
        WriteData = 3, //（写） 往控制表里写入字符
        RegWrite = 4, //(异步写) 类似于WRITE DATA，但是控制字符写入后并不马上动作，直到ACTION指令到达
        Action = 5, //（执行异步写） 触发REG WRITE写入的动作
        SyncWriteData = 0x83, //（同步写） 用于同时控制多个舵机
        Reset = 6, //（复位） 把控制表复位为出厂值
    };

    /*
     * @brief 接收状态机
     */
    enum class RecvStatusMachine : uint8_t
    {
        RecvFirstHeader = 1, //0xff
        RecvSecondHeader = 2, //0xff
        RecvID = 3,
        RecvDataLen = 4,
        RecvError = 5, //返回的应答包包含舵机的当前状态 ERROR，若舵机当前工作状态不正常，会通过这个字节反映出来（各状态所代表意义详见手册内存控制表），若ERROR为0，则舵机无报错信息。
        RecvData = 6,
        RecvCheckSum = 7,
        RecvDone = 8,
    };

    /*
     * @brief 舵机波特率选项
     */
    enum class Baud : uint8_t
    {
        Baud1000000 = 0,
        Baud500000 = 1,
        Baud250000 = 2,
        Baud128000 = 3,
        Baud115200 = 4,
        Baud76800 = 5,
        Baud57600 = 6,
        Baud38400 = 7,
    };

    /*
     * @brief 舵机内存表
     */
    enum class MemoryTable : uint8_t
    {
        FirmwareMajorVersion = 0x0, //固件主版本号
        FirmwareMinorVersion = 0x1, //固件次版本号
        End = 0x2, //END
        ServoMajorVersion = 0x3, //舵机主版本号
        ServoMinorVersion = 0x4, //舵机次版本号
        ID = 0x5, //ID 总线上唯一的身份识别码，同一总线不可出现重复ID号 254号(OxFE)为广播ID，广播不返回应答包
        Baud = 0x6, //波特率 0-7分别代表波特率如下 1000000，500000，250000，128000，115200，76800，57600，38400
        ReturnDelay = 0x7, //返回延时 最小单位为2us,最大可设置返回延时 254*2=508us(STS此地址无功能)
        ResponseStatusLevel = 0x8, //相位2（应答状态级别）0:除读指令与PING指令外其它指令不返回应答包 1:对所有指令返回应答包
        MinAngleLimit = 0x9, //最小角度限制 设置运动行程最小值限制，取值小于最大角度限制，多圈绝对位置控制时此值为0
        MaxAngleLimit = 0xb, //最大角度限制 设置运动行程最大值限制，取值大于最小角度限制，多圈绝对位置控制时此值为0
        MaxTempLimit = 0xd, //最高温度上限 最高工作温度限制，如设置为70 则最高温度为70摄氏度，设置精度为1摄氏度
        MaxInputVoltage = 0xe, //最高输入电压 最高输入电压如设置为80，则最高工作电压限制为8.0V，设置精度为0.1V
        MinInputVoltage = 0xf, //最低输入电压 最低输入电压如设置为40，则最低工作电压限制为4.0V，设置精度为0.1V
        MaxTorque = 0x10, //最大扭矩 设置舵机的最大输出扭矩限制，设1000 = 100% * 堵转扭力，上电赋值给48号地址转矩限制
        UnloadCondition = 0x13, //卸载条件 对应位设置1为开启相应保护,对应位设置0为关闭相应保护,详见特殊字节位解析
        LEDAlarmCondition = 0x14, //LED报警条件 对应位设置1为开启闪灯报警,对应位设置0为关闭闪灯报警,详见特殊字节位解析
        PosPIDkP = 0x15, //位置环P 比例系数 控制电机的比例系数
        PosPIDkD = 0x16, //位置环D 微分系数 控制电机的微分系数
        PosPIDkI = 0x17, //位置环I 积分系数 控制电机的积分系数
        MinStartForce = 0x18, //最小启动力 设置舵机的最小输出启动扭矩，设10 = 1% * 堵转扭力
        IntegralLimit = 0x19, //积分限制值 最大积分值=积分限制值*4,0表示关闭积分限制功能，位置模式0与步进模式3生效
        ClockwiseInsensitiveZone = 0x1a, //顺时针不灵敏区 最小单位为一个最小分辨角度
        CounterclockwiseInsensitiveZone = 0x1b, //逆时针不灵敏区 最小单位为一个最小分辨角度
        ProtectCurrent = 0x1c, //保护电流 最大可设置电流为 500 * 6.5mA= 3250mA
        AngleResolution = 0x1e, //角度分辨率 对传感器最小分辨角度（度/步）的放大系数，修改此值可以扩展控制圈数 多圈控制时需修改地址0x12参数BIT4置1，当前位置反馈值会被修改为大角度反馈
        PositionCorrection = 0x1f, //位置校正 BIT11为方向位，表示正负方向，其他位可表示范围为0-2047步
        RunMode = 0x21, //运行模式 0：位置伺服模式
                                //1：电机恒速模式,用参数0x2E运行速度参数控制，最高位BIT15为方向位
                                //2：PWM开环调速度模式，用参数0x2C 运行时间参数控制，BIT10为方向位
                                //3：步进伺服模式，用参数0x2A 目标位置表示步进步数，最高位BIT15为方向位
                                //工作在模式3时，必须把0x9与0xB 最小最大角度限制设为0，否则不能无限步进
        ProtectTorque = 0x22,  //保护扭矩 进入过载保护后输出扭矩，如设20表示20%的最大扭矩
        ProtectTime = 0x23, //保护时间 当前负载输出超过过载扭力并保持的计时时长，如设200表示2秒，最大可设2.5秒
        OverloadTorque = 0x24, //过载扭矩 启动过载保护时间计时的最大扭力阀值，如设80表示80%的最大扭矩
        SpeedPIDkP = 0x25, //速度闭环P比例系数 电机恒速模式（模式1）下,速度环比例系数
        OvercurrentProtectTime = 0x26, //过流保护时间 最大可设置254 * 10ms = 2540ms
        SpeedPIDkI = 0x27, //速度闭环I积分系数 电机恒速模式（模式1）下,速度环积分系数（变动备注：速度闭环I积分系数缩小10倍较3.6版本）
        TorqueSwitch = 0x28, //扭矩开关 写0：关闭扭力输出； 写1：打开扭力输出；写128：任意当前位置较正为2048
        Acceleration = 0x29, //加速度 如设置为10 则按1000步每秒平方的加减速度变速
        TargetPosition = 0x2a, //目标位置 每步为一个最小分辨角度，绝对位置控制方式，最大对应最大有效角度
        RunTime = 0x2c, //运行时间 PWM开环调速度模式下，取值范围50-1000，BIT10为方向位
        RunSpeed = 0x2e, //运行速度 单位时间（每秒）内运动的步数，50步/秒= 0.732 RPM(圈每分钟）
        TorqueLimit = 0x30, //转矩限制上电初始值会由最大扭矩（0x10）赋值，用户可程序修改此值来控制最大扭矩的输出
        LockFlag = 0x37, //锁标志 写0关闭写入锁，写入EPROM地址的值掉电保存 写1打开写入锁，写入EPROM地址的值掉电不保存
        CurrentPostion = 0x38, //当前位置 反馈当前所处位置的步数，每步为一个最小分辨角度；绝对位置控制方式，最大值对应最大有效角度
        CurrentSpeed = 0x3a, //当前速度 反馈当前电机转动的速度，单位时间（每秒）内运动的步数
        CurrentLoad = 0x3c, //当前负载 当前控制输出驱动电机的电压占空比
        CurrentVoltage = 0x3e, //当前电压 当前舵机工作电压
        CurrentTemp = 0x3f, //当前温度 当前舵机内部工作温度
        AsyncWriteFlag = 0x40, //异步写标志 采用异步写指令时，标志位
        ServoStatus = 0x41, //舵机状态 对应位被置1表示相应错误出现,对应位0为无相应该错误,详见特殊字节位解析
        MoveFlag = 0x42, //移动标志 舵机在运动时标志为1，舵机停止时为0
        Current = 0x45, //当前电流 最大可测量电流为 500 * 6.5mA= 3250mA
    };


    /*
     * @brief 校验和, 对每个字节数据求和，最后结果取反
     * @param data 数据指针
     * @aram len 数据长度
     * @return sum
     */
    uint8_t checksum(uint8_t* data, std::size_t len);

    /*
     * @brief 将数据编码成数据包
     * @param buff 数据包缓冲区，要求大小能够存储数据的长度+5
     * @param id 舵机id
     * @param cmd 控制指令
     * @param data 数据指针
     * @param len 数据长度
     * @return 数据包长度
     */
    std::size_t encode(uint8_t* buff, uint8_t id, ControlCMD cmd, uint8_t* data, std::size_t len);

    /*
     * @brief 将数据包解码成数据
     * @param buff 数据包缓冲区
     * @param buff_len 数据包长度
     * @param id 输出舵机id
     * @param error 输出当前状态
     * @param data 输出数据包指针
     * @param data_len 输出数据长度
     * @return 是否解码成功,成功返回0
     */
    int decode(uint8_t* buff, std::size_t buff_len, uint8_t* id, uint8_t* error, uint8_t** data, std::size_t* data_len);

    /*
     * @brief 接收响应数据包
     * @param buff 接收数据缓冲区
     * @param buff_len 缓冲区大小
     * @param package_len 接收到的数据包大小
     * @param timeout 超时时间
     * @return 是否成功接收，成功返回0，不成功返回-1
     */
    int recv_package(uint8_t* buff, std::size_t buff_len, std::size_t* package_len, int timeout);

public:

    /*
     * @brief 检查舵机是否在线
     * @param id 舵机id
     * @param ms 超时时间
     */
    bool ping(uint8_t id, int ms);

    /*
     * @brief 舵机数据读取
     * @param id 舵机id
     * @param addr 舵机内存控制表地址
     * @param data_len 要读取的数据长度
     * @param data 接收数据缓冲区
     * @return 是否读取成功
     */
    bool read_data(uint8_t id, uint8_t addr, uint8_t* data, uint8_t data_len);

    /*
     * @brief 舵机数据写入
     * @param id 舵机id
     * @param addr 舵机内存控制表地址
     * @param data 数据
     * @param data_len 写入数据长度
     * @return 是否写入成功
     */
    bool write_data(uint8_t id, uint8_t addr, uint8_t* data, uint8_t data_len);

    /*
     * @brief 异步写指令
     * @param id 舵机id
     * @param addr 舵机内存控制表地址
     * @param data 数据
     * @param data_len 写入数据长度
     * @return 是否写入成功
     */
    bool reg_write(uint8_t id, uint8_t addr, uint8_t* data, uint8_t data_len);

    /*
     * @brief 执行异步写指令
     * @details 在控制多个舵机时，使用ACTION指令可以使第一个和最后一个舵机同时执行各自的动作，中间无延时。
     */
    bool action();

    /*
     * @brief 复位指令
     * @param id 舵机id
     * @details 把内存控制表里特定的数据进行复位（特定舵机型号采用）
     */
    bool reset(uint8_t id);
};