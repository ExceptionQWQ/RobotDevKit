/*
 * @author BusyBox
 * @date 2024/5/25
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 * @details https://fashionrobo.com/
 */

#include "rdk/core/servo/fashion_star/fashion_star_protocol.h"

FashionStarProtocol::FashionStarProtocol(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
}

FashionStarProtocol::~FashionStarProtocol()
{

}

/*
 * @brief 校验和, 对每个字节数据求和
 * @param data 数据指针
 * @aram len 数据长度
 * @return sum
 */
uint8_t FashionStarProtocol::checksum(uint8_t* data, std::size_t len)
{
    uint8_t sum = 0;
    for (std::size_t i = 0; i != len; ++i) sum += data[i];
    return sum % 256;
}

/*
 * @brief 将数据编码成数据包
 * @param buff 数据包缓冲区，要求大小能够存储数据的长度+5
 * @param id 舵机id
 * @param data 数据指针
 * @param len 数据长度
 * @return 数据包长度
 */
std::size_t FashionStarProtocol::encode(uint8_t* buff, ControlCMD cmd, uint8_t* data, std::size_t len)
{
    if (len > 240) return 0;
    buff[0] = 0x12;
    buff[1] = 0x4c;
    buff[2] = static_cast<uint8_t>(cmd);
    buff[3] = len;
    memcpy(buff + 4, data, len);
    uint8_t sum = checksum(buff, 4 + len);
    buff[4 + len] = sum;
    return 4 + len + 1;
}

/*
 * @brief 将数据包解码成数据
 * @param buff 数据包缓冲区
 * @param buff_size 数据包长度
 * @param id 输出舵机id
 * @param data 输出数据包指针
 * @param data_len 输出数据长度
 * @return 是否解码成功,成功返回0
 */
int FashionStarProtocol::decode(uint8_t* buff, std::size_t buff_len, ControlCMD* cmd, uint8_t** data, std::size_t* data_len)
{
    if (buff[0] != 0x05 || buff[1] != 0x1c) return -1;
    *cmd = static_cast<ControlCMD>(buff[2]);
    *data_len = buff[3];
    if (buff_len < 4 + *data_len + 1) return -2;
    *data = buff + 4;
    uint8_t sum = checksum(buff, 4 + *data_len);
    if (sum != buff[4 + *data_len]) return -3;
    return 0;
}

/*
 * @brief 接收响应数据包
 * @param buff 接收数据缓冲区
 * @param buff_len 缓冲区大小
 * @param package_len 接收到的数据包大小
 * @param timeout 超时时间
 * @return 是否成功接收，成功返回0，不成功返回-1
 */
int FashionStarProtocol::recv_package(uint8_t* buff, std::size_t buff_len, std::size_t* package_len, int timeout)
{
    RecvStatusMachine status = RecvStatusMachine::RecvFirstHeader;
    std::size_t recv_bytes = 0;
    std::size_t index = 0;
    std::size_t data_len = 0;
    std::size_t remain_len = 0;
    while (true) {
        if (status == RecvStatusMachine::RecvDone) break;
        switch (status) {
            case RecvStatusMachine::RecvFirstHeader:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                if (buff[index] == 0x05) {
                    ++index;
                    status = RecvStatusMachine::RecvSecondHeader;
                }
                break;
            case RecvStatusMachine::RecvSecondHeader:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                if (buff[index] == 0x1c) {
                    ++index;
                    status = RecvStatusMachine::RecvCmdID;
                } else {
                    index = 0;
                    status = RecvStatusMachine::RecvFirstHeader;
                }
                break;
            case RecvStatusMachine::RecvCmdID:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                if (buff[index] >= 1 && buff[index] <= 17) {
                    ++index;
                    status = RecvStatusMachine::RecvDataLen;
                } else {
                    index = 0;
                    status = RecvStatusMachine::RecvFirstHeader;
                }
                break;
            case RecvStatusMachine::RecvDataLen:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                data_len = buff[index];
                ++index;
                status = RecvStatusMachine::RecvData;
                break;
            case RecvStatusMachine::RecvData:
                remain_len = data_len;
                while (remain_len) {
                    recv_bytes = io_stream->read(buff + index, remain_len, timeout);
                    if (recv_bytes == 0) return -1; //超时
                    remain_len -= recv_bytes;
                    index += recv_bytes;
                }
                status = RecvStatusMachine::RecvCheckSum;
                break;
            case RecvStatusMachine::RecvCheckSum:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                ++index;
                status = RecvStatusMachine::RecvDone;
                break;
        }
    }
    *package_len = index;
    return 0;
}

/*
 * @brief 检查舵机是否在线
 * @param id 舵机id
 * @param ms 超时时间
 */
bool FashionStarProtocol::ping(uint8_t id, int ms)
{
    uint8_t buff[16];
    std::size_t len = encode(buff, ControlCMD::PING, &id, 1);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 16, &package_len, ms) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t data_len;
        ControlCMD cmd;
        uint8_t* data;
        if (decode(buff, 16, &cmd, &data, &data_len) != 0) continue;
        if (data_len == 1 && data[0] == id) return true;
    }
    return false;
}

/*
 * @brief 重新将用户自定义数据设置为默认值
 * @param id 舵机id
 */
bool FashionStarProtocol::reset_user_data(uint8_t id)
{
    uint8_t buff[16];
    std::size_t len = encode(buff, ControlCMD::RESET_USER_DATA, &id, 1);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 16, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t data_len;
        ControlCMD cmd;
        uint8_t* data;
        if (decode(buff, 16, &cmd, &data, &data_len) != 0) continue;
        if (data_len == 2 && data[0] == id && data[1] == 1) return true;
    }
    return false;
}

/*
 * @brief 舵机数据读取
 * @param id 舵机id
 * @param data_id 数据id
 * @param data 接收数据缓冲区
 * @param data_len 接收数据长度
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_data(uint8_t id, DataID data_id, uint8_t* data, std::size_t* data_len)
{
    uint8_t buff[16];
    uint8_t request[2];
    request[0] = id;
    request[1] = static_cast<uint8_t>(data_id);
    std::size_t len = encode(buff, ControlCMD::READ_DATA, request, 2);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 16, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t recv_data_len;
        ControlCMD cmd;
        uint8_t* recv_data;
        if (decode(buff, 16, &cmd, &recv_data, &recv_data_len) != 0) continue;
        if (recv_data_len > 2 && recv_data[0] == id && recv_data[1] == static_cast<uint8_t>(data_id)) {
            memcpy(data, recv_data + 2, recv_data_len - 2);
            *data_len = recv_data_len - 2;
            return true;
        }
    }
    return false;
}

/*
 * @brief 舵机数据写入
 * @param id 舵机id
 * @param data_id 数据id
 * @param data 数据
 * @param data_len 数据长度
 * @return 是否写入成功
 */
bool FashionStarProtocol::write_data(uint8_t id, DataID data_id, uint8_t* data, std::size_t data_len)
{
    uint8_t buff[16];
    uint8_t request[8];
    request[0] = id;
    request[1] = static_cast<uint8_t>(data_id);
    memcpy(request + 2, data, data_len);
    std::size_t len = encode(buff, ControlCMD::WRITE_DATA, request, 2 + data_len);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 16, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t recv_data_len;
        ControlCMD cmd;
        uint8_t* recv_data;
        if (decode(buff, 16, &cmd, &recv_data, &recv_data_len) != 0) continue;
        if (recv_data_len == 3 && recv_data[0] == id && recv_data[1] == static_cast<uint8_t>(data_id) && recv_data[2] == 1) {
            return true;
        }
    }
    return false;
}

/*
 * @brief 舵机电压读取
 * @param id 舵机id
 * @param voltage 电压 mV
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_voltage(uint8_t id, uint16_t* voltage)
{
    uint8_t buff[2];
    std::size_t data_len;
    if (read_data(id, DataID::Voltage, buff, &data_len) && data_len == 2) {
        *voltage = *reinterpret_cast<uint16_t*>(buff);
        return true;
    }
    return false;
}

/*
 * @brief 舵机电流读取
 * @param id 舵机id
 * @param current 电流 mA
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_current(uint8_t id, uint16_t* current)
{
    uint8_t buff[2];
    std::size_t data_len;
    if (read_data(id, DataID::Current, buff, &data_len) && data_len == 2) {
        *current = *reinterpret_cast<uint16_t*>(buff);
        return true;
    }
    return false;
}

/*
 * @brief 舵机功率读取
 * @param id 舵机id
 * @param power 功率 mW
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_power(uint8_t id, uint16_t* power)
{
    uint8_t buff[2];
    std::size_t data_len;
    if (read_data(id, DataID::Power, buff, &data_len) && data_len == 2) {
        *power = *reinterpret_cast<uint16_t*>(buff);
        return true;
    }
    return false;
}

/*
 * @brief 舵机温度读取
 * @param id 舵机id
 * @param temp 温度
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_temp(uint8_t id, uint16_t* temp)
{
    uint8_t buff[2];
    std::size_t data_len;
    if (read_data(id, DataID::Temprature, buff, &data_len) && data_len == 2) {
        *temp = *reinterpret_cast<uint16_t*>(buff);
        double adc = *temp;
        double R1 = 10000;
        double Rt25 = 10000;
        double B = 3435;
        double K = 273.15;
        double temprature = 1 / (std::log(R1 * adc / Rt25 / (4096 - adc)) / B + 1 / (K + 25))- K;
        *temp = temprature;
        return true;
    }
    return false;
}

/*
 * @brief 舵机工作状态读取
 * @param id 舵机id
 * @param status 状态
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_servo_status(uint8_t id, uint8_t* status)
{
    uint8_t buff[1];
    std::size_t data_len;
    if (read_data(id, DataID::ServoStatus, buff, &data_len) && data_len == 1) {
        *status = *reinterpret_cast<uint8_t*>(buff);
        return true;
    }
    return false;
}

/*
 * @brief 舵机型号读取
 * @param id 舵机id
 * @param type 型号
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_servo_type(uint8_t id, uint16_t* type)
{
    uint8_t buff[2];
    std::size_t data_len;
    if (read_data(id, DataID::ServoType, buff, &data_len) && data_len == 2) {
        *type = *reinterpret_cast<uint16_t*>(buff);
        return true;
    }
    return false;
}

/*
 * @brief 舵机固件版本读取
 * @param id 舵机id
 * @param version 固件版本
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_firmware_version(uint8_t id, uint16_t* version)
{
    uint8_t buff[2];
    std::size_t data_len;
    if (read_data(id, DataID::FirmwareVersion, buff, &data_len) && data_len == 2) {
        *version = *reinterpret_cast<uint16_t*>(buff);
        return true;
    }
    return false;
}

/*
 * @brief 舵机序列号读取
 * @param id 舵机id
 * @param number 舵机序列号
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_serial_number(uint8_t id, uint32_t* number)
{
    uint8_t buff[4];
    std::size_t data_len;
    if (read_data(id, DataID::SerialNumber, buff, &data_len) && data_len == 4) {
        *number = *reinterpret_cast<uint32_t*>(buff);
        return true;
    }
    return false;
}

/*
 * @brief 设置舵机id
 * @param id 舵机id
 * @param new_id 新的舵机id
 * @return 是否写入成功
 */
bool FashionStarProtocol::set_servo_id(uint8_t id, uint8_t new_id)
{
    if (write_data(id, DataID::ServoID, &new_id, 1)) {
        return true;
    }
    return false;
}

/*
 * @brief 设置舵机波特率
 * @param id 舵机id
 * @param baud 波特率
 * @return 是否写入成功
 */
bool FashionStarProtocol::set_servo_baud(uint8_t id, Baud baud)
{
    if (write_data(id, DataID::Baudrate, reinterpret_cast<uint8_t*>(&baud), 1)) {
        return true;
    }
    return false;
}

/*
 * @brief 设置舵机堵转保护模式
 * @param id 舵机id
 * @param status 0 将舵机功率降低到功率上限 1 释放舵机锁力（舵机卸力）
 * @return 是否写入成功
 */
bool FashionStarProtocol::set_stall_protect_mode(uint8_t id, bool status)
{
    if (write_data(id, DataID::StallProtectMode, reinterpret_cast<uint8_t*>(&status), 1)) {
        return true;
    }
    return false;
}

/*
 * @brief 设置舵机启动加速度处理开关
 * @param id 舵机id
 * @param status 0 不启动加速度处理 1 启用加速度处理
 * @return 是否写入成功
 * @attention 舵机目前必须设置启用加速度处理，即只能设置0x01这个选项。
 * @details 舵机速度控制算法 根据时间t推算加速度，行程前t/4加速，行程中间t/2保持匀速，行程后t/4减速。
 */
bool FashionStarProtocol::set_accel_switch(uint8_t id, bool status)
{
    if (write_data(id, DataID::AccelSwitch, reinterpret_cast<uint8_t*>(&status), 1)) {
        return true;
    }
    return false;
}

/*
 * @brief 设置舵机上电锁力开关
 * @param id 舵机id
 * @param status 0 上电舵机释放锁力 1 上电舵机保持锁力
 * @return 是否写入成功
 * @details 上电舵机保持锁力 的意思是，舵机上电时，读取舵机当前所在的角度，并让舵机维持在这一角度。释放舵机锁力 的意思是，不再驱动舵机维持在一个特定的角度，此时可以自由转动舵机的主轴。
 */
bool FashionStarProtocol::set_po_lock_switch(uint8_t id, bool status)
{
    if (write_data(id, DataID::PoLockSwitch, reinterpret_cast<uint8_t*>(&status), 1)) {
        return true;
    }
    return false;
}

/*
 * @brief 设置轮式模式刹车开关
 * @param id 舵机id
 * @param status 0 停止时释放舵机锁力 1 停止时保持舵机锁力
 * @return 是否写入成功
 */
bool FashionStarProtocol::set_wb_lock_switch(uint8_t id, bool status)
{
    if (write_data(id, DataID::WbLockSwitch, reinterpret_cast<uint8_t*>(&status), 1)) {
        return true;
    }
    return false;
}

/*
 * @brief 设置角度限制开关
 * @param id 舵机id
 * @param status 0 关闭角度限制 1 开启角度限制
 * @return 是否写入成功
 * @attention 建议在主控上做舵机角度约束。
 */
bool FashionStarProtocol::set_angle_limit_switch(uint8_t id, bool status)
{
    if (write_data(id, DataID::AngleLimitSwitch, reinterpret_cast<uint8_t*>(&status), 1)) {
        return true;
    }
    return false;
}

/*
 * @brief 设置上电首次缓慢执行
 * @param id 舵机id
 * @param status 0 关闭上电首次缓慢执行 1 开启上电首次缓慢执行
 * @return 是否写入成功
 */
bool FashionStarProtocol::set_soft_start_switch(uint8_t id, bool status)
{
    if (write_data(id, DataID::SoftStartSwitch, reinterpret_cast<uint8_t*>(&status), 1)) {
        return true;
    }
    return false;
}

/*
 * @brief 设置舵机角度
 * @param id 舵机id
 * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
 * @param interval 到达目标角度的时间 (单位ms)
 * @param power 执行功率(单位mW) 如果power=0或者大于功率保持值，按照功率保持值操作
 */
bool FashionStarProtocol::move_on_angle_move_mode(uint8_t id, int16_t angle, uint16_t interval, uint16_t power)
{
    uint8_t buff[16];
    uint8_t request[8];
    request[0] = id;
    *reinterpret_cast<int16_t*>(request + 1) = angle;
    *reinterpret_cast<uint16_t*>(request + 3) = interval;
    *reinterpret_cast<uint16_t*>(request + 5) = power;
    std::size_t len = encode(buff, ControlCMD::MOVE_ON_ANGLE_MODE, request, 7);
    io_stream->write(buff, len);
    return true;
}

/*
 * @brief 读取舵机角度
 * @param id 舵机id
 * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
 * @return 是否读取成功
 */
bool FashionStarProtocol::read_angle(uint8_t id, int16_t* angle)
{
    uint8_t buff[16];
    std::size_t len = encode(buff, ControlCMD::READ_ANGLE, &id, 1);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 16, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t data_len;
        ControlCMD cmd;
        uint8_t* data;
        if (decode(buff, 16, &cmd, &data, &data_len) != 0) continue;
        if (data_len == 3 && data[0] == id) {
            *angle = *reinterpret_cast<int16_t*>(data + 1);
            return true;
        }
    }
    return false;
}

/*
 * @brief 舵机角度控制-指定速度
 * @param id 舵机id
 * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
 * @param target_velocity 目标速度
 * @param acc_interval 单位：ms，启动时加速段的时间
 * @param dec_interval 单位：ms，运动到接近目标的减速段时间
 * @param power 执行功率(单位mW)
 */
bool FashionStarProtocol::move_on_angle_mode_ex_by_velocity(uint8_t id, int16_t angle, uint16_t target_velocity, uint16_t acc_interval, uint16_t dec_interval, uint16_t power)
{
    uint8_t buff[32];
    uint8_t request[16];
    request[0] = id;
    *reinterpret_cast<int16_t*>(request + 1) = angle;
    *reinterpret_cast<uint16_t*>(request + 3) = target_velocity;
    *reinterpret_cast<uint16_t*>(request + 5) = acc_interval;
    *reinterpret_cast<uint16_t*>(request + 7) = dec_interval;
    *reinterpret_cast<uint16_t*>(request + 9) = power;
    std::size_t len = encode(buff, ControlCMD::MOVE_ON_ANGLE_MODE_EX_BY_VELOCITY, request, 11);
    io_stream->write(buff, len);
    return true;
}

/*
 * @brief 舵机角度控制-指定时间
 * @param id 舵机id
 * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
 * @param interval 到达目标角度的时间 (单位ms)
 * @param acc_interval 单位：ms，启动时加速段的时间
 * @param dec_interval 单位：ms，运动到接近目标的减速段时间
 * @param power 执行功率(单位mW)
 */
bool FashionStarProtocol::move_on_angle_mode_ex_by_interval(uint8_t id, int16_t angle, uint16_t interval, uint16_t acc_interval, uint16_t dec_interval, uint16_t power)
{
    uint8_t buff[32];
    uint8_t request[16];
    request[0] = id;
    *reinterpret_cast<int16_t*>(request + 1) = angle;
    *reinterpret_cast<uint16_t*>(request + 3) = interval;
    *reinterpret_cast<uint16_t*>(request + 5) = acc_interval;
    *reinterpret_cast<uint16_t*>(request + 7) = dec_interval;
    *reinterpret_cast<uint16_t*>(request + 9) = power;
    std::size_t len = encode(buff, ControlCMD::MOVE_ON_ANGLE_MODE_EX_BY_INTERVAL, request, 11);
    io_stream->write(buff, len);
    return true;
}

/*
 * @brief 阻尼模式控制
 * @param id 舵机id
 * @param power 执行功率(单位mW)
 */
bool FashionStarProtocol::move_on_damping_mode(uint8_t id, uint16_t power)
{
    uint8_t buff[32];
    uint8_t request[16];
    request[0] = id;
    *reinterpret_cast<uint16_t*>(request + 1) = power;
    std::size_t len = encode(buff, ControlCMD::MOVE_ON_DAMPING_MODE, request, 3);
    io_stream->write(buff, len);
    return true;
}

/*
 * @brief 多圈角度模式
 * @attention 仅适用于磁编码舵机
 * @param id 舵机id
 * @param angle 舵机角度，数值正负代表方向(单位0.1度)，数值范围：-3686400～3686400
 * @param interval 到达目标角度的时间 (单位ms)，最大值为 4096000
 * @param power 执行功率(单位mW)， 如果power=0或者大于功率保持值，按照功率保持值操作
 */
bool FashionStarProtocol::move_on_multi_turn_angle_mode(uint8_t id, int16_t angle, uint16_t interval, uint16_t power)
{
    uint8_t buff[32];
    uint8_t request[16];
    request[0] = id;
    *reinterpret_cast<int16_t*>(request + 1) = angle;
    *reinterpret_cast<uint16_t*>(request + 3) = interval;
    *reinterpret_cast<uint16_t*>(request + 5) = power;
    std::size_t len = encode(buff, ControlCMD::MOVE_ON_MULTI_TURN_ANGLE_MODE, request, 7);
    io_stream->write(buff, len);
    return true;
}

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
bool FashionStarProtocol::move_on_multi_turn_angle_mode_ex_by_interval(uint8_t id, int16_t angle, uint16_t interval, uint16_t acc_interval, uint16_t dec_interval, uint16_t power)
{
    uint8_t buff[32];
    uint8_t request[16];
    request[0] = id;
    *reinterpret_cast<int16_t*>(request + 1) = angle;
    *reinterpret_cast<uint16_t*>(request + 3) = interval;
    *reinterpret_cast<uint16_t*>(request + 5) = acc_interval;
    *reinterpret_cast<uint16_t*>(request + 7) = dec_interval;
    *reinterpret_cast<uint16_t*>(request + 9) = power;
    std::size_t len = encode(buff, ControlCMD::MOVE_ON_MULTI_TURN_ANGLE_MODE_EX_BY_INTERVAL, request, 11);
    io_stream->write(buff, len);
    return true;
}

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
bool FashionStarProtocol::move_on_multi_turn_angle_mode_ex_by_velocity(uint8_t id, int16_t angle, uint16_t target_velocity, uint16_t acc_interval, uint16_t dec_interval, uint16_t power)
{
    uint8_t buff[32];
    uint8_t request[16];
    request[0] = id;
    *reinterpret_cast<int16_t*>(request + 1) = angle;
    *reinterpret_cast<uint16_t*>(request + 3) = target_velocity;
    *reinterpret_cast<uint16_t*>(request + 5) = acc_interval;
    *reinterpret_cast<uint16_t*>(request + 7) = dec_interval;
    *reinterpret_cast<uint16_t*>(request + 9) = power;
    std::size_t len = encode(buff, ControlCMD::MOVE_ON_MULTI_TURN_ANGLE_MODE_EX_BY_VELOCITY, request, 11);
    io_stream->write(buff, len);
    return true;
}

/*
 * @brief 读取多圈角度
 * @attention 仅适用于磁编码舵机
 * @param id 舵机id
 * @param angle 舵机角度，数值正负代表方向( 单位0.1度，举例angle=900 就等于90度)
 * @param turn 转动圈数
 */
bool FashionStarProtocol::read_multi_turn_angle(uint8_t id, int16_t* angle, uint16_t* turn)
{
    uint8_t buff[16];
    std::size_t len = encode(buff, ControlCMD::READ_MULTI_TURN_ANGLE, &id, 1);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 16, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t data_len;
        ControlCMD cmd;
        uint8_t* data;
        if (decode(buff, 16, &cmd, &data, &data_len) != 0) continue;
        if (data_len == 5 && data[0] == id) {
            *angle = *reinterpret_cast<int16_t*>(data + 1);
            *turn = *reinterpret_cast<uint16_t*>(data + 3);
            return true;
        }
    }
    return false;
}

/*
 * @brief 重设多圈角度
 * @attention 仅适用于磁编码舵机
 * @param id 舵机id
 */
bool FashionStarProtocol::reset_multi_turn_angle(uint8_t id)
{
    uint8_t buff[32];
    uint8_t request[16];
    request[0] = id;
    std::size_t len = encode(buff, ControlCMD::RESET_MULTI_TURN_ANGLE, request, 1);
    io_stream->write(buff, len);
    return true;
}