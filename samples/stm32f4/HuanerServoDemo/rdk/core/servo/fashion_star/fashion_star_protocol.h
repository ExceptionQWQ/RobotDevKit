/*
 * @author BusyBox
 * @date 2024/5/25
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 * @details https://fashionrobo.com/
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <functional>
#include <chrono>
#include <cmath>
#include <string>
#include "rdk/core/transfer/io_stream.h"



class FashionStarProtocol
{
public:
    /*
     * @brief 舵机波特率选项
     */
    enum class Baud : uint8_t
    {
        Baud9600 = 1,
        Baud19200 = 2,
        Baud38400 = 3,
        Baud57600 = 4,
        Baud115200 = 5,
        Baud250000 = 6,
        Baud500000 = 7,
        Baud1000000 = 8,
    };


    FashionStarProtocol(std::shared_ptr<IOStream> io_stream);
    ~FashionStarProtocol();


    /*
     * @brief 检查舵机是否在线
     * @param id 舵机id
     * @param ms 超时时间
     */
    bool ping(uint8_t id, int ms);

    /*
     * @brief 重新将用户自定义数据设置为默认值
     * @param id 舵机id
     */
    bool reset_user_data(uint8_t id);


private:
    std::shared_ptr<IOStream> io_stream;
    int timeout = 10;

    /*
     * 串口舵机共支持17种舵机控制指令。其中12种为通用指令，余下5种指令仅限于***磁编码系列舵机***
     */
    enum class ControlCMD : uint8_t
    {
        PING = 1, //舵机通讯检测	检查舵机是否在线
        RESET_USER_DATA = 2, //用户自定义数据重置	重新将用户自定义数据设置为默认值
        READ_DATA = 3, //舵机数据读取	读取单个用户自定义参数，用户自定义参数的字节长度为可变长
        WRITE_DATA = 4, //用户自定义数据写	写入单个用户自定义数据，用户自定义参数的字节长度为可变长
        READ_BATCH_DATA = 5, //用户自定义数据批量读取	读取所有的用户自定义数据，所有的用户自定义数据的字节长度为32字节
        WRITE_BATCH_DATA = 6, //用户自定义数据批量写入	写入所有的用户自定义数据，所有的用户自定义数据的字节长度为32字节
        MOVE_ON_WHEEL_MODE = 7, //轮式模式控制	设置旋转方向，定速，定圈，定时
        MOVE_ON_ANGLE_MODE = 8, //角度模式控制-设置舵机角度	设置舵机旋转的目标角度，时间间隔，执行功率
        MOVE_ON_DAMPING_MODE = 9, //阻尼模式控制	阻尼模式，设置舵机保持的功率
        READ_ANGLE = 10, //读取舵机角度
        MOVE_ON_ANGLE_MODE_EX_BY_INTERVAL = 11, //角度模式控制-指定时间	带加减速控制的角度模式，设置加减速时间，时间间隔，目标角度，执行功率
        MOVE_ON_ANGLE_MODE_EX_BY_VELOCITY = 12, //角度模式控制-指定速度	带加减速控制的角度模式，设置加减速时间，目标速度，目标角度，执行功率
        /*
         * 余下5种指令仅限于***磁编码系列舵机***
         */
        MOVE_ON_MULTI_TURN_ANGLE_MODE = 13, //多圈角度控制	设置舵机旋转的目标角度，时间间隔，执行功率
        MOVE_ON_MULTI_TURN_ANGLE_MODE_EX_BY_INTERVAL = 14, //多圈角度控制-指定时间	带加减速控制的多圈角度模式，设置加减速时间，时间间隔，目标角度，执行功率
        MOVE_ON_MULTI_TURN_ANGLE_MODE_EX_BY_VELOCITY = 15, //多圈角度控制-指定速度	带加减速控制的多圈角度模式，设置加减速时间，目标速度，目标角度，执行功率
        READ_MULTI_TURN_ANGLE = 16, //读取多圈角度
        RESET_MULTI_TURN_ANGLE = 17, //重设多圈角度	重置圈数，重置后角度范围为[-180，180]
    };

    /*
     * @brief 1~8为只读数据
     * @details https://wiki.fashionrobo.com/uartbasic/uart-protocol/#621
     */
    enum class DataID : uint8_t
    {
        Voltage = 1, //舵机电压	uint16_t mV
        Current = 2, //舵机电流	uint16_t mA
        Power = 3, //舵机动率	uint16_t mW
        Temprature = 4, //舵机温度	uint16_t ADC
        ServoStatus = 5, //舵机工作状态	uint8_t
        ServoType = 6, //舵机型号	uint16_t
        FirmwareVersion = 7, //舵机固件版本	uint16_t
        SerialNumber = 8, //舵机序列号	uint32_t
        /*
         * 以下为自定义数据
         */
        ResponseSwitch = 33, //响应开关	uint8_t
        ServoID = 34, //舵机ID	uint8_t
        Baudrate = 36, //波特率选项	uint8_t
        StallProtectMode = 37, //舵机堵转保护模式	uint8_t
        StallPowerLimit = 38, //舵机堵转功率上限	uint16_t mW
        OverVoltLow = 39, //舵机电压下限	uint16_t	mV
        OverVoltHigh = 40, //舵机电压上限   uint16_t	mV
        OverTemprature = 41, //温度上限	uint16_t ADC
        OverPower = 42, //功率上限	uint16_t	mW
        OverCurrent = 43, //电流上限	uint16_t mA
        AccelSwitch = 44, //加速度处理开关	uint8_t
        PoLockSwitch = 46, //舵机上电锁力开关	uint8_t
        WbLockSwitch = 47, //轮式刹车锁力开关	uint8_t
        AngleLimitSwitch = 48, //角度限制开关	uint8_t
        SoftStartSwitch	= 49, //上电首次缓慢执行	uint8_t
        SoftStartTime = 50, //上电首次执行时间	uint16_t	ms
        AngleLimitHigh = 51, //舵机角度上限	int16_t	0.1度
        AngleLimitLow = 52, //舵机角度下限 int16_t 0.1度
        AngleMidOffset = 53, //舵机中位角度偏移 int16_t 0.1度
    };

    /*
     * @brief 接收状态机
     */
    enum class RecvStatusMachine : uint8_t
    {
        RecvFirstHeader = 1, //0x05
        RecvSecondHeader = 2, //0x5c
        RecvCmdID = 3,
        RecvDataLen = 4,
        RecvData = 5,
        RecvCheckSum = 6,
        RecvDone = 7,
    };

    /*
     * @brief 校验和, 对每个字节数据求和
     * @param data 数据指针
     * @aram len 数据长度
     * @return sum
     */
    uint8_t checksum(uint8_t* data, std::size_t len);

    /*
     * @brief 将数据编码成数据包
     * @param buff 数据包缓冲区，要求大小能够存储数据的长度+5
     * @param id 舵机id
     * @param data 数据指针
     * @param len 数据长度
     * @return 数据包长度
     */
    std::size_t encode(uint8_t* buff, ControlCMD cmd, uint8_t* data, std::size_t len);

    /*
     * @brief 将数据包解码成数据
     * @param buff 数据包缓冲区
     * @param buff_size 数据包长度
     * @param id 输出舵机id
     * @param data 输出数据包指针
     * @param data_len 输出数据长度
     * @return 是否解码成功,成功返回0
     */
    int decode(uint8_t* buff, std::size_t buff_len, ControlCMD* cmd, uint8_t** data, std::size_t* data_len);

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
     * @brief 舵机数据读取
     * @param id 舵机id
     * @param data_id 数据id
     * @param data 接收数据缓冲区
     * @param data_len 接收数据长度
     * @return 是否读取成功
     */
    bool read_data(uint8_t id, DataID data_id, uint8_t* data, std::size_t* data_len);

    /*
     * @brief 舵机数据写入
     * @param id 舵机id
     * @param data_id 数据id
     * @param data 数据
     * @param data_len 数据长度
     * @return 是否写入成功
     */
    bool write_data(uint8_t id, DataID data_id, uint8_t* data, std::size_t data_len);

    /*
     * @brief 舵机电压读取
     * @param id 舵机id
     * @param voltage 电压 mV
     * @return 是否读取成功
     */
    bool read_voltage(uint8_t id, uint16_t* voltage);

    /*
     * @brief 舵机电流读取
     * @param id 舵机id
     * @param current 电流 mA
     * @return 是否读取成功
     */
    bool read_current(uint8_t id, uint16_t* current);

    /*
     * @brief 舵机功率读取
     * @param id 舵机id
     * @param power 功率 mW
     * @return 是否读取成功
     */
    bool read_power(uint8_t id, uint16_t* power);

    /*
     * @brief 舵机温度读取
     * @param id 舵机id
     * @param temp 温度
     * @return 是否读取成功
     */
    bool read_temp(uint8_t id, uint16_t* temp);

    /*
     * @brief 舵机工作状态读取
     * @param id 舵机id
     * @param status 状态
     * @return 是否读取成功
     */
    bool read_servo_status(uint8_t id, uint8_t* status);

    /*
     * @brief 舵机型号读取
     * @param id 舵机id
     * @param type 型号
     * @return 是否读取成功
     */
    bool read_servo_type(uint8_t id, uint16_t* type);

    /*
     * @brief 舵机固件版本读取
     * @param id 舵机id
     * @param version 固件版本
     * @return 是否读取成功
     */
    bool read_firmware_version(uint8_t id, uint16_t* version);

    /*
     * @brief 舵机序列号读取
     * @param id 舵机id
     * @param number 舵机序列号
     * @return 是否读取成功
     */
    bool read_serial_number(uint8_t id, uint32_t* number);

    /*
     * @brief 设置舵机id
     * @param id 舵机id
     * @param new_id 新的舵机id
     * @return 是否写入成功
     */
    bool set_servo_id(uint8_t id, uint8_t new_id);

    /*
     * @brief 设置舵机波特率
     * @param id 舵机id
     * @param baud 波特率
     * @return 是否写入成功
     */
    bool set_servo_baud(uint8_t id, Baud baud);

    /*
     * @brief 设置舵机堵转保护模式
     * @param id 舵机id
     * @param status 0 将舵机功率降低到功率上限 1 释放舵机锁力（舵机卸力）
     * @return 是否写入成功
     */
    bool set_stall_protect_mode(uint8_t id, bool status);

    /*
     * @brief 设置舵机启动加速度处理开关
     * @param id 舵机id
     * @param status 0 不启动加速度处理 1 启用加速度处理
     * @return 是否写入成功
     * @attention 舵机目前必须设置启用加速度处理，即只能设置0x01这个选项。
     * @details 舵机速度控制算法 根据时间t推算加速度，行程前t/4加速，行程中间t/2保持匀速，行程后t/4减速。
     */
    bool set_accel_switch(uint8_t id, bool status);

    /*
     * @brief 设置舵机上电锁力开关
     * @param id 舵机id
     * @param status 0 上电舵机释放锁力 1 上电舵机保持锁力
     * @return 是否写入成功
     * @details 上电舵机保持锁力 的意思是，舵机上电时，读取舵机当前所在的角度，并让舵机维持在这一角度。释放舵机锁力 的意思是，不再驱动舵机维持在一个特定的角度，此时可以自由转动舵机的主轴。
     */
    bool set_po_lock_switch(uint8_t id, bool status);

    /*
     * @brief 设置轮式模式刹车开关
     * @param id 舵机id
     * @param status 0 停止时释放舵机锁力 1 停止时保持舵机锁力
     * @return 是否写入成功
     */
    bool set_wb_lock_switch(uint8_t id, bool status);

    /*
     * @brief 设置角度限制开关
     * @param id 舵机id
     * @param status 0 关闭角度限制 1 开启角度限制
     * @return 是否写入成功
     * @attention 建议在主控上做舵机角度约束。
     */
    bool set_angle_limit_switch(uint8_t id, bool status);

    /*
     * @brief 设置上电首次缓慢执行
     * @param id 舵机id
     * @param status 0 关闭上电首次缓慢执行 1 开启上电首次缓慢执行
     * @return 是否写入成功
     */
    bool set_soft_start_switch(uint8_t id, bool status);

    /*
     * @brief 设置舵机角度
     * @param id 舵机id
     * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
     * @param interval 到达目标角度的时间 (单位ms)
     * @param power 执行功率(单位mW) 如果power=0或者大于功率保持值，按照功率保持值操作
     */
    bool move_on_angle_move_mode(uint8_t id, int16_t angle, uint16_t interval, uint16_t power);

    /*
     * @brief 读取舵机角度
     * @param id 舵机id
     * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
     * @return 是否读取成功
     */
    bool read_angle(uint8_t id, int16_t* angle);

    /*
     * @brief 舵机角度控制-指定速度
     * @param id 舵机id
     * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
     * @param target_velocity 目标速度
     * @param acc_interval 单位：ms，启动时加速段的时间
     * @param dec_interval 单位：ms，运动到接近目标的减速段时间
     * @param power 执行功率(单位mW)
     */
    bool move_on_angle_mode_ex_by_velocity(uint8_t id, int16_t angle, uint16_t target_velocity, uint16_t acc_interval, uint16_t dec_interval, uint16_t power);

    /*
     * @brief 舵机角度控制-指定时间
     * @param id 舵机id
     * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
     * @param interval 到达目标角度的时间 (单位ms)
     * @param acc_interval 单位：ms，启动时加速段的时间
     * @param dec_interval 单位：ms，运动到接近目标的减速段时间
     * @param power 执行功率(单位mW)
     */
    bool move_on_angle_mode_ex_by_interval(uint8_t id, int16_t angle, uint16_t interval, uint16_t acc_interval, uint16_t dec_interval, uint16_t power);

    /*
     * @brief 阻尼模式控制
     * @param id 舵机id
     * @param power 执行功率(单位mW)
     */
    bool move_on_damping_mode(uint8_t id, uint16_t power);

    /*
     * @brief 多圈角度模式
     * @attention 仅适用于磁编码舵机
     * @param id 舵机id
     * @param angle 舵机角度，数值正负代表方向(单位0.1度)，数值范围：-3686400～3686400
     * @param interval 到达目标角度的时间 (单位ms)，最大值为 4096000
     * @param power 执行功率(单位mW)， 如果power=0或者大于功率保持值，按照功率保持值操作
     */
    bool move_on_multi_turn_angle_mode(uint8_t id, int16_t angle, uint16_t interval, uint16_t power);

    /*
     * @brief 多圈角度控制-指定时间
     * @attention 仅适用于磁编码舵机
     * @param id 舵机id
     * @param angle 舵机角度，数值正负代表方向(单位0.1度)，数值范围：-3686400～3686400
     * @param interval 到达目标角度的时间 (单位ms)，最大值为 4096000
     * @param acc_interval 单位：ms，启动时加速段的时间
     * @param dec_interval 单位：ms，运动到接近目标的减速段时间
     * @param power 执行功率(单位mW)， 如果power=0或者大于功率保持值，按照功率保持值操作
     */
    bool move_on_multi_turn_angle_mode_ex_by_interval(uint8_t id, int16_t angle, uint16_t interval, uint16_t acc_interval, uint16_t dec_interval, uint16_t power);

    /*
     * @brief 多圈角度控制-指定速度
     * @attention 仅适用于磁编码舵机
     * @param id 舵机id
     * @param angle 舵机角度，数值正负代表方向(单位0.1度)，数值范围：-3686400～3686400
     * @param target_velocity 目标速度
     * @param acc_interval 单位：ms，启动时加速段的时间
     * @param dec_interval 单位：ms，运动到接近目标的减速段时间
     * @param power 执行功率(单位mW)， 如果power=0或者大于功率保持值，按照功率保持值操作
     */
    bool move_on_multi_turn_angle_mode_ex_by_velocity(uint8_t id, int16_t angle, uint16_t target_velocity, uint16_t acc_interval, uint16_t dec_interval, uint16_t power);

    /*
     * @brief 读取多圈角度
     * @attention 仅适用于磁编码舵机
     * @param id 舵机id
     * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
     * @param turn 转动圈数
     */
    bool read_multi_turn_angle(uint8_t id, int16_t* angle, uint16_t* turn);

    /*
     * @brief 重设多圈角度
     * @attention 仅适用于磁编码舵机
     * @param id 舵机id
     */
    bool reset_multi_turn_angle(uint8_t id);

};